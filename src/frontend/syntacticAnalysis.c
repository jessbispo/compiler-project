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


/**
 * @brief Parse a simple expression term (identifier or literal)
 */
static ASTNode* parseTerm(Parser *parser, int *index) {
    Token *current = &parser->tokens[*index];
    
    /* Type 10 = TOKEN_IDENTIFIER */
    if (current->type == 10) {
        ASTNode *node = createASTNode("IDENTIFIER", current->lexeme, current->line);
        (*index)++;
        return node;
    }
    
    /* Type 12 = TOKEN_LITERAL */
    if (current->type == 12) {
        ASTNode *node = createASTNode("LITERAL", current->lexeme, current->line);
        (*index)++;
        return node;
    }
    
    /* Type 11 = TOKEN_KEYWORD - handle boolean literals */
    if (current->type == 11) {
        if (strcmp(current->lexeme, "true") == 0 || strcmp(current->lexeme, "false") == 0) {
            ASTNode *node = createASTNode("LITERAL", current->lexeme, current->line);
            (*index)++;
            return node;
        }
    }
    
    return NULL;
}


static ASTNode* parseExpression(Parser *parser, int *index) {
    ASTNode *left = parseTerm(parser, index);
    if (!left) return NULL;
    
    /* Type 0 = TOKEN_EOF, Type 14 = TOKEN_DELIMITER */
    while (parser->tokens[*index].type != 0 &&
           parser->tokens[*index].type != 14 &&
           strcmp(parser->tokens[*index].lexeme, ";") != 0 &&
           strcmp(parser->tokens[*index].lexeme, ")") != 0) {
        
        Token *op = &parser->tokens[*index];
        
        const char *op_type = NULL;
        
        if (op->type == 1) {
            if (strcmp(op->lexeme, "+") == 0) op_type = "PLUS";
            else if (strcmp(op->lexeme, "-") == 0) op_type = "MINUS";
            else if (strcmp(op->lexeme, "*") == 0) op_type = "TIMES";
            else if (strcmp(op->lexeme, "/") == 0) op_type = "DIVIDE";
        } 
        else if (op->type == 11 && strcmp(op->lexeme, "div") == 0) {
            op_type = "DIV_OP";
        } else if (op->type == 11 && strcmp(op->lexeme, "mod") == 0) {
            op_type = "MOD_OP";
        } 
        else if (op->type == 2) {
            if (strcmp(op->lexeme, "<") == 0) op_type = "LT";
            else if (strcmp(op->lexeme, ">") == 0) op_type = "GT";
            else if (strcmp(op->lexeme, "<=") == 0) op_type = "LE";
            else if (strcmp(op->lexeme, ">=") == 0) op_type = "GE";
            else if (strcmp(op->lexeme, "=") == 0) op_type = "EQ";
            else if (strcmp(op->lexeme, "<>") == 0) op_type = "NE";
        } 
       
        else if (op->type == 4) {
            if (strcmp(op->lexeme, "and") == 0) op_type = "AND";
            else if (strcmp(op->lexeme, "or") == 0) op_type = "OR";
        }
        
        if (!op_type) break;
        
        (*index)++;
        
        ASTNode *right = parseTerm(parser, index);
        if (!right) {
            break;
        }
        
        ASTNode *op_node = createASTNode(op_type, NULL, op->line);
        if (op_node) {
            if (op_node->line <= 0 || op_node->line > 10000) {
                op_node->line = left->line; 
            }
            astAddChild(op_node, left);
            astAddChild(op_node, right);
            left = op_node;
        }
    }
    
    return left;
}


static ASTNode* parseCondition(Parser *parser, int *index) {
    if (parser->tokens[*index].type == 14 &&
        strcmp(parser->tokens[*index].lexeme, "(") == 0) {
        (*index)++;
    }
    
    ASTNode *condition = parseExpression(parser, index);
    
    if (parser->tokens[*index].type == 14 &&
        strcmp(parser->tokens[*index].lexeme, ")") == 0) {
        (*index)++;
    }
    
    return condition;
}

static ASTNode* parseBlock(Parser *parser, int *index);

static ASTNode* parseStatement(Parser *parser, int *index) {
    Token *stmt_token = &parser->tokens[*index];
    
    if (stmt_token->type == 10) {
        ASTNode *assign = createASTNode("ASSIGN", NULL, stmt_token->line);
        if (!assign) return NULL;
        
        ASTNode *lhs = createASTNode("IDENTIFIER", stmt_token->lexeme, stmt_token->line);
        if (lhs) astAddChild(assign, lhs);
        
        (*index)++;
        
        int found_assign = 0;
        
        if (parser->tokens[*index].type == 3 ||
            (parser->tokens[*index].type != 0 && 
             strcmp(parser->tokens[*index].lexeme, ":=") == 0)) {
            (*index)++;
            found_assign = 1;
        }
        else if (parser->tokens[*index].type == 14 && 
                 strcmp(parser->tokens[*index].lexeme, ":") == 0 &&
                 parser->tokens[*index + 1].type == 2 &&
                 strcmp(parser->tokens[*index + 1].lexeme, "=") == 0) {
            (*index) += 2; 
            found_assign = 1;
        }
        
        if (found_assign) {
            ASTNode *rhs = parseExpression(parser, index);
            if (rhs) astAddChild(assign, rhs);
        }
        
        if (parser->tokens[*index].type == 14 &&
            strcmp(parser->tokens[*index].lexeme, ";") == 0) {
            (*index)++;
        }
        
        return assign;
    }
    
    if (stmt_token->type == 11 && strcmp(stmt_token->lexeme, "if") == 0) {
        (*index)++;
        
        ASTNode *ifNode = createASTNode("IF", NULL, stmt_token->line);
        if (!ifNode) return NULL;
        
        ASTNode *condition = parseCondition(parser, index);
        if (condition) astAddChild(ifNode, condition);
        

        if (parser->tokens[*index].type == 11 &&
            strcmp(parser->tokens[*index].lexeme, "then") == 0) {
            (*index)++;
        }
        
        ASTNode *thenBlock = NULL;
        if (parser->tokens[*index].type == 11 &&
            strcmp(parser->tokens[*index].lexeme, "begin") == 0) {
            thenBlock = parseBlock(parser, index);
        } else {
            thenBlock = parseStatement(parser, index);
        }
        if (thenBlock) astAddChild(ifNode, thenBlock);
        
 
        if (parser->tokens[*index].type == 11 &&
            strcmp(parser->tokens[*index].lexeme, "else") == 0) {
            (*index)++;
            
            ASTNode *elseBlock = NULL;
            if (parser->tokens[*index].type == 11 &&
                strcmp(parser->tokens[*index].lexeme, "begin") == 0) {
                elseBlock = parseBlock(parser, index);
            } else {
                elseBlock = parseStatement(parser, index);
            }
            if (elseBlock) astAddChild(ifNode, elseBlock);
        }
        
        return ifNode;
    }
    
    if (stmt_token->type == 11 && strcmp(stmt_token->lexeme, "while") == 0) {
        (*index)++;
        
        ASTNode *whileNode = createASTNode("WHILE", NULL, stmt_token->line);
        if (!whileNode) return NULL;
        
        ASTNode *condition = parseCondition(parser, index);
        if (condition) astAddChild(whileNode, condition);
        
        if (parser->tokens[*index].type == 11 &&
            strcmp(parser->tokens[*index].lexeme, "do") == 0) {
            (*index)++;
        }
        
        ASTNode *body = NULL;
        if (parser->tokens[*index].type == 11 &&
            strcmp(parser->tokens[*index].lexeme, "begin") == 0) {
            body = parseBlock(parser, index);
        } else {
            body = parseStatement(parser, index);
        }
        if (body) astAddChild(whileNode, body);
        
        return whileNode;
    }
    
    if (stmt_token->type == 11 && strcmp(stmt_token->lexeme, "write") == 0) {
        ASTNode *writeNode = createASTNode("WRITE", NULL, stmt_token->line);
        if (!writeNode) return NULL;
        
        (*index)++;
        
        if (parser->tokens[*index].type == 14 &&
            strcmp(parser->tokens[*index].lexeme, "(") == 0) {
            (*index)++;
            
            if (parser->tokens[*index].type == 10 ||
                parser->tokens[*index].type == 12) {
                ASTNode *arg = createASTNode("ARGUMENT",
                                            parser->tokens[*index].lexeme,
                                            parser->tokens[*index].line);
                if (arg) astAddChild(writeNode, arg);
                (*index)++;
            }
            
            if (parser->tokens[*index].type == 14 &&
                strcmp(parser->tokens[*index].lexeme, ")") == 0) {
                (*index)++;
            }
        }
        
        if (parser->tokens[*index].type == 14 &&
            strcmp(parser->tokens[*index].lexeme, ";") == 0) {
            (*index)++;
        }
        
        return writeNode;
    }
    
    if (stmt_token->type == 11 && strcmp(stmt_token->lexeme, "read") == 0) {
        ASTNode *readNode = createASTNode("READ", NULL, stmt_token->line);
        if (!readNode) return NULL;
        
        (*index)++;
        

        if (parser->tokens[*index].type == 14 &&
            strcmp(parser->tokens[*index].lexeme, "(") == 0) {
            (*index)++;
            
            if (parser->tokens[*index].type == 10) {
                ASTNode *arg = createASTNode("ARGUMENT",
                                            parser->tokens[*index].lexeme,
                                            parser->tokens[*index].line);
                if (arg) astAddChild(readNode, arg);
                (*index)++;
            }
            
            if (parser->tokens[*index].type == 14 &&
                strcmp(parser->tokens[*index].lexeme, ")") == 0) {
                (*index)++;
            }
        }
        
        if (parser->tokens[*index].type == 14 &&
            strcmp(parser->tokens[*index].lexeme, ";") == 0) {
            (*index)++;
        }
        
        return readNode;
    }
    
    (*index)++;
    return NULL;
}

static ASTNode* parseBlock(Parser *parser, int *index) {
    if (parser->tokens[*index].type != 11 ||
        strcmp(parser->tokens[*index].lexeme, "begin") != 0) {
        return NULL;
    }
    
    Token *begin_token = &parser->tokens[*index];
    (*index)++;
    
    ASTNode *block = createASTNode("BLOCK", NULL, begin_token->line);
    if (!block) return NULL;
    
    while (parser->tokens[*index].type != 0 &&
           !(parser->tokens[*index].type == 11 &&
             strcmp(parser->tokens[*index].lexeme, "end") == 0)) {
        
        ASTNode *stmt = parseStatement(parser, index);
        if (stmt) astAddChild(block, stmt);
    }
    
    if (parser->tokens[*index].type == 11 &&
        strcmp(parser->tokens[*index].lexeme, "end") == 0) {
        (*index)++;
    }
    
    if (parser->tokens[*index].type == 14 &&
        strcmp(parser->tokens[*index].lexeme, ";") == 0) {
        (*index)++;
    }
    
    return block;
}
int syntacticAnalysis(Parser *parser) {
    if (!parser || !parser->tokens) {
        return 1;
    }
    
    if (parser->root == NULL) {
        parser->root = createASTNode("PROGRAM", NULL, -1);
        if (!parser->root) return 1;
    }

    int i = 0;
    
    while (parser->tokens[i].type != 0 && 
           !(parser->tokens[i].type == 11 &&
             strcmp(parser->tokens[i].lexeme, "program") == 0)) {
        i++;
    }
    
    while (parser->tokens[i].type != 0) {
        Token *current = &parser->tokens[i];
        if (current->type == 11 && strcmp(current->lexeme, "var") == 0) {
            ASTNode *varDecl = createASTNode("VAR_DECL", NULL, current->line);
            if (varDecl) {
                i++;
                while (parser->tokens[i].type != 0 && 
                       strcmp(parser->tokens[i].lexeme, ":") != 0) {
                    if (parser->tokens[i].type == 10) {
                        ASTNode *idNode = createASTNode("IDENTIFIER", 
                                                       parser->tokens[i].lexeme, 
                                                       parser->tokens[i].line);
                        if (idNode) astAddChild(varDecl, idNode);
                    }
                    i++;
                }
                if (parser->tokens[i].type != 0 && 
                    strcmp(parser->tokens[i].lexeme, ":") == 0) {
                    i++;
                    if (parser->tokens[i].type == 11 ||
                        parser->tokens[i].type == 13 ||
                        parser->tokens[i].type == 10) {
                        ASTNode *typeNode = createASTNode("TYPE", 
                                                         parser->tokens[i].lexeme, 
                                                         parser->tokens[i].line);
                        if (typeNode) astAddChild(varDecl, typeNode);
                    }
                }
                astAddChild(parser->root, varDecl);
            }
        }
        
        else if (current->type == 11 && strcmp(current->lexeme, "procedure") == 0) {
            i++;
            ASTNode *procDecl = createASTNode("PROCEDURE", NULL, current->line);
            if (procDecl && parser->tokens[i].type == 10) {
                ASTNode *nameNode = createASTNode("IDENTIFIER", 
                                                 parser->tokens[i].lexeme, 
                                                 parser->tokens[i].line);
                if (nameNode) astAddChild(procDecl, nameNode);
            }
            astAddChild(parser->root, procDecl);
        }
        
        else if (current->type == 11 && strcmp(current->lexeme, "function") == 0) {
            i++;
            ASTNode *funcDecl = createASTNode("FUNCTION", NULL, current->line);
            if (funcDecl && parser->tokens[i].type == 10) {
                ASTNode *nameNode = createASTNode("IDENTIFIER", 
                                                 parser->tokens[i].lexeme, 
                                                 parser->tokens[i].line);
                if (nameNode) astAddChild(funcDecl, nameNode);
            }
            astAddChild(parser->root, funcDecl);
        }
        
        else if (current->type == 11 && strcmp(current->lexeme, "begin") == 0) {
            ASTNode *block = parseBlock(parser, &i);
            if (block) {
                astAddChild(parser->root, block);
            }
            continue; 
        }
        
        i++;
    }

    return 0;
}