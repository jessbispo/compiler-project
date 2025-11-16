#include <stdio.h>
#include "syntacticAnalysis.h"
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

int syntacticAnalysis(Parser *parser) {
    if (!parser || !parser->tokens) {
        return 1;  /* error */
    }
    
    /* Minimal behavior: create an empty PROGRAM root node so later stages
     * (semantic analysis) can run even if parser is not yet implemented.
     * The real parser should replace this with the actual AST.
     */
    if (parser->root == NULL) {
        parser->root = createASTNode("PROGRAM", NULL, -1);
        if (!parser->root) return 1;
    }

    /* Currently no syntactic checks performed here; return success. */
    return 0;
}
