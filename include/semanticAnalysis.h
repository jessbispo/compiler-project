#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include "syntacticAnalysis.h"
#include "symbolTable.h"
#include "errorHandler.h"

/*
 * Entry point for semantic analysis.
 * Returns 0 when no semantic errors were detected, non-zero otherwise.
 */
int semanticAnalysis(ASTNode *root, SymbolTable **symtab, ErrorTable *errtab, const char *filename);

#endif /* SEMANTIC_ANALYSIS_H */
