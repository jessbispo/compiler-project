#include <stdio.h>
#include "syntacticAnalysis.h"
#include "fileHandler.h"
#include <stdlib.h>
#include <string.h>

Parser createParser(Token *tokens, SymbolTable **symtab, ErrorTable *errtab) {
    Parser parser;
    parser.tokens = tokens;
    parser.current = 0;
    parser.symtab = symtab;
    parser.errtab = errtab;
    parser.error_count = 0;
    parser.root = NULL;
    return parser;
}

ASTNode* createASTNode(const char *type, const char *value, int line) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = type ? strdup(type) : NULL;
    node->value = value ? strdup(value) : NULL;
    node->line = line;
    node->child_count = 0;
    node->child_capacity = 4;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * node->child_capacity);
    if (!node->children) {
        free(node->type);
        free(node->value);
        free(node);
        return NULL;
    }
    return node;
}

void astAddChild(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;
    if (parent->child_count >= parent->child_capacity) {
        int newcap = parent->child_capacity * 2;
        ASTNode **tmp = (ASTNode**)realloc(parent->children, sizeof(ASTNode*) * newcap);
        if (!tmp) return; /* keep old children on failure */
        parent->children = tmp;
        parent->child_capacity = newcap;
    }
    parent->children[parent->child_count++] = child;
}

void freeAST(ASTNode *node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; ++i) {
        freeAST(node->children[i]);
    }
    free(node->children);
    free(node->type);
    free(node->value);
    free(node);
}

/**
 * @brief Recursively print AST node and its children
 * @param node AST node to print
 * @param indent Current indentation level
 * @param file File pointer for output (NULL for stdout)
 */
static void printASTNode(ASTNode *node, int indent, FILE *file) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < indent; i++) {
        if (file) fprintf(file, "  ");
        else printf("  ");
    }
    
    // Print node info
    if (file) {
        fprintf(file, "├─ [%s]", node->type);
        if (node->value) fprintf(file, " = \"%s\"", node->value);
        if (node->line > 0) fprintf(file, " (line %d)", node->line);
        fprintf(file, "\n");
    } else {
        printf("├─ [%s]", node->type);
        if (node->value) printf(" = \"%s\"", node->value);
        if (node->line > 0) printf(" (line %d)", node->line);
        printf("\n");
    }
    
    // Print children recursively
    for (int i = 0; i < node->child_count; i++) {
        printASTNode(node->children[i], indent + 1, file);
    }
}

/**
 * @brief Print AST to stdout
 * @param node Root node of AST
 */
void printAST(ASTNode *node) {
    if (!node) {
        printf("Nenhuma árvore sintática gerada.\n");
        return;
    }
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("ÁRVORE SINTÁTICA (AST)\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printASTNode(node, 0, NULL);
    printf("\n");
}

/**
 * @brief Print AST to file
 * @param node Root node of AST
 * @param filepath Path to output file
 */
void printASTToFile(ASTNode *node, const char *filepath) {
    if (!node || !filepath) return;
    
    FILE *file = NULL;
    if (openFile(&file, filepath, "w")) return;
    if (!file) return;
    
    fprintf(file, "═══════════════════════════════════════════════════════════════\n");
    fprintf(file, "ÁRVORE SINTÁTICA (AST)\n");
    fprintf(file, "═══════════════════════════════════════════════════════════════\n");
    printASTNode(node, 0, file);
    
    closeFile(file);
}

int syntacticAnalysis(Parser *parser) {
    if (!parser || !parser->tokens) {
        return 1;  /* error */
    }
    
    /* Create PROGRAM root node */
    if (parser->root == NULL) {
        parser->root = createASTNode("PROGRAM", NULL, -1);
        if (!parser->root) return 1;
    }

    /* Build a simple AST from tokens by grouping them into logical sections
     * This is a simplified parser that creates nodes for major program sections
     */
    
    int i = 0;
    
    /* Skip initial tokens until we find 'program' keyword */
    while (parser->tokens[i].type != TOKEN_EOF && 
           parser->tokens[i].type != TOKEN_KEYWORD) {
        i++;
    }
    
    /* Parse variable declarations and procedure/function declarations */
    while (parser->tokens[i].type != TOKEN_EOF) {
        Token *current = &parser->tokens[i];
        
        /* Variable declaration */
        if (current->type == TOKEN_KEYWORD && strcmp(current->lexeme, "var") == 0) {
            ASTNode *varDecl = createASTNode("VAR_DECL", NULL, current->line);
            if (varDecl) {
                /* Collect variable names and type */
                i++;
                while (parser->tokens[i].type != TOKEN_EOF && 
                       strcmp(parser->tokens[i].lexeme, ":") != 0) {
                    if (parser->tokens[i].type == TOKEN_IDENTIFIER) {
                        ASTNode *idNode = createASTNode("IDENTIFIER", 
                                                       parser->tokens[i].lexeme, 
                                                       parser->tokens[i].line);
                        if (idNode) astAddChild(varDecl, idNode);
                    }
                    i++;
                }
                /* Get the type */
                if (parser->tokens[i].type != TOKEN_EOF && 
                    strcmp(parser->tokens[i].lexeme, ":") == 0) {
                    i++;
                    if (parser->tokens[i].type == TOKEN_KEYWORD ||
                        parser->tokens[i].type == TOKEN_DATATYPE ||
                        parser->tokens[i].type == TOKEN_IDENTIFIER) {
                        ASTNode *typeNode = createASTNode("TYPE", 
                                                         parser->tokens[i].lexeme, 
                                                         parser->tokens[i].line);
                        if (typeNode) astAddChild(varDecl, typeNode);
                    }
                }
                astAddChild(parser->root, varDecl);
            }
        }
        
        /* Procedure declaration */
        else if (current->type == TOKEN_KEYWORD && strcmp(current->lexeme, "procedure") == 0) {
            i++;
            ASTNode *procDecl = createASTNode("PROCEDURE", NULL, current->line);
            if (procDecl && parser->tokens[i].type == TOKEN_IDENTIFIER) {
                ASTNode *nameNode = createASTNode("IDENTIFIER", 
                                                 parser->tokens[i].lexeme, 
                                                 parser->tokens[i].line);
                if (nameNode) astAddChild(procDecl, nameNode);
            }
            astAddChild(parser->root, procDecl);
        }
        
        /* Function declaration */
        else if (current->type == TOKEN_KEYWORD && strcmp(current->lexeme, "function") == 0) {
            i++;
            ASTNode *funcDecl = createASTNode("FUNCTION", NULL, current->line);
            if (funcDecl && parser->tokens[i].type == TOKEN_IDENTIFIER) {
                ASTNode *nameNode = createASTNode("IDENTIFIER", 
                                                 parser->tokens[i].lexeme, 
                                                 parser->tokens[i].line);
                if (nameNode) astAddChild(funcDecl, nameNode);
            }
            astAddChild(parser->root, funcDecl);
        }
        
        /* Begin block */
        else if (current->type == TOKEN_KEYWORD && strcmp(current->lexeme, "begin") == 0) {
            ASTNode *block = createASTNode("BLOCK", NULL, current->line);
            if (block) {
                i++;
                /* Collect statements until 'end' */
                while (parser->tokens[i].type != TOKEN_EOF &&
                       !(parser->tokens[i].type == TOKEN_KEYWORD && 
                         strcmp(parser->tokens[i].lexeme, "end") == 0)) {
                    
                    Token *stmt_token = &parser->tokens[i];
                    
                    /* Assignment statement: identifier := expression */
                    if (stmt_token->type == TOKEN_IDENTIFIER) {
                        ASTNode *assign = createASTNode("ASSIGN", NULL, stmt_token->line);
                        if (assign) {
                            ASTNode *lhs = createASTNode("IDENTIFIER", 
                                                        stmt_token->lexeme, 
                                                        stmt_token->line);
                            if (lhs) astAddChild(assign, lhs);
                            
                            i++;
                            /* Skip := operator */
                            if (parser->tokens[i].type != TOKEN_EOF &&
                                strcmp(parser->tokens[i].lexeme, ":=") == 0) {
                                i++;
                                /* Parse right-hand side expression */
                                while (parser->tokens[i].type != TOKEN_EOF &&
                                       parser->tokens[i].type != TOKEN_DELIMITER &&
                                       strcmp(parser->tokens[i].lexeme, ";") != 0) {
                                    if (parser->tokens[i].type == TOKEN_IDENTIFIER ||
                                        parser->tokens[i].type == TOKEN_LITERAL ||
                                        parser->tokens[i].type == TOKEN_KEYWORD) {
                                        ASTNode *rhs = createASTNode("VALUE", 
                                                                    parser->tokens[i].lexeme, 
                                                                    parser->tokens[i].line);
                                        if (rhs) astAddChild(assign, rhs);
                                    }
                                    i++;
                                }
                            }
                            astAddChild(block, assign);
                        }
                        continue;
                    }
                    
                    /* If statement */
                    if (stmt_token->type == TOKEN_KEYWORD && 
                        strcmp(stmt_token->lexeme, "if") == 0) {
                        ASTNode *ifNode = createASTNode("IF", NULL, stmt_token->line);
                        if (ifNode) astAddChild(block, ifNode);
                    }
                    
                    /* While loop */
                    else if (stmt_token->type == TOKEN_KEYWORD && 
                            strcmp(stmt_token->lexeme, "while") == 0) {
                        ASTNode *whileNode = createASTNode("WHILE", NULL, stmt_token->line);
                        if (whileNode) astAddChild(block, whileNode);
                    }
                    
                    /* Write statement */
                    else if (stmt_token->type == TOKEN_KEYWORD && 
                            strcmp(stmt_token->lexeme, "write") == 0) {
                        ASTNode *writeNode = createASTNode("WRITE", NULL, stmt_token->line);
                        if (writeNode) {
                            i++;
                            /* Skip '(' */
                            if (parser->tokens[i].type == TOKEN_DELIMITER &&
                                strcmp(parser->tokens[i].lexeme, "(") == 0) {
                                i++;
                                if (parser->tokens[i].type == TOKEN_IDENTIFIER ||
                                    parser->tokens[i].type == TOKEN_LITERAL) {
                                    ASTNode *arg = createASTNode("ARGUMENT",
                                                                parser->tokens[i].lexeme,
                                                                parser->tokens[i].line);
                                    if (arg) astAddChild(writeNode, arg);
                                    i++;
                                }
                                /* Skip ')' */
                                if (parser->tokens[i].type == TOKEN_DELIMITER &&
                                    strcmp(parser->tokens[i].lexeme, ")") == 0) {
                                    i++;
                                }
                            }
                            astAddChild(block, writeNode);
                        }
                        continue;
                    }
                    
                    /* Read statement */
                    else if (stmt_token->type == TOKEN_KEYWORD && 
                            strcmp(stmt_token->lexeme, "read") == 0) {
                        ASTNode *readNode = createASTNode("READ", NULL, stmt_token->line);
                        if (readNode) {
                            i++;
                            /* Skip '(' */
                            if (parser->tokens[i].type == TOKEN_DELIMITER &&
                                strcmp(parser->tokens[i].lexeme, "(") == 0) {
                                i++;
                                if (parser->tokens[i].type == TOKEN_IDENTIFIER) {
                                    ASTNode *arg = createASTNode("ARGUMENT",
                                                                parser->tokens[i].lexeme,
                                                                parser->tokens[i].line);
                                    if (arg) astAddChild(readNode, arg);
                                    i++;
                                }
                                /* Skip ')' */
                                if (parser->tokens[i].type == TOKEN_DELIMITER &&
                                    strcmp(parser->tokens[i].lexeme, ")") == 0) {
                                    i++;
                                }
                            }
                            astAddChild(block, readNode);
                        }
                        continue;
                    }
                    
                    i++;
                }
                astAddChild(parser->root, block);
            }
        }
        
        i++;
    }

    /* Currently no syntactic checks performed here; return success. */
    return 0;
}
