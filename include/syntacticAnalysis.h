#ifndef SYNTACTICAL_ANALYSIS_H
#define SYNTACTICAL_ANALYSIS_H

#include "lexicalAnalysis.h"
#include "symbolTable.h"
#include "errorHandler.h"



/**
 * @brief Generic AST node used by the syntactic analysis phase
 */
typedef struct ASTNode {
    char *type;              /* node type label (e.g. "PROGRAM", "VAR", "IF") */
    char *value;             /* optional literal or identifier value */
    int line;                /* source line where node originates (or -1) */
    struct ASTNode **children;
    int child_count;
    int child_capacity;
} ASTNode;

/* AST helpers */
ASTNode* createASTNode(const char *type, const char *value, int line);
void astAddChild(ASTNode *parent, ASTNode *child);
void freeAST(ASTNode *node);
void printAST(ASTNode *node);
void printASTToFile(ASTNode *node, const char *filepath);

/**
 * @brief Structure to manage parser state
 */
typedef struct {
    Token *tokens;           
    int current;             
    SymbolTable **symtab;    
    ErrorTable *errtab;      
    int error_count;
    ASTNode *root;           /* raiz da AST gerada pela análise sintática */
} Parser;

/**
 * @brief Initialize parser with token stream
 * @param tokens Token array from lexical analysis
 * @param symtab Symbol table from lexical analysis
 * @param errtab Error table for error reporting
 * @return Initialized Parser structure
 */
Parser createParser(Token *tokens, SymbolTable **symtab, ErrorTable *errtab);

/**
 * @brief Perform syntactic analysis
 * @param parser Parser structure
 * @return 0 if successful, 1 if there are syntactic errors
 */
int syntacticAnalysis(Parser *parser);

#endif /* SYNTACTICAL_ANALYSIS_H */