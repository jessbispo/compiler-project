/** @brief Módulo responsável pela análise semântica do código MiniPascal.
 *  @details Este módulo implementa a análise semântica do código-fonte MiniPascal, verificando tipos, escopos e outras regras semânticas.
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862)
 */

#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include "syntacticAnalysis.h"
#include "symbolTable.h"
#include "errorHandler.h"

/** @brief Função para realizar a análise semântica do código MiniPascal.
 *  @param root Ponteiro para o nó raiz da Árvore Sintática Abstrata (AST).
 *  @param symtab Ponteiro para a tabela de símbolos.
 *  @param errtab Ponteiro para a tabela de erros.
 *  @param filename Nome do arquivo fonte sendo analisado.
 *  @return 0 se a análise semântica for bem-sucedida, 1 caso contrário.
 */
int semanticAnalysis(ASTNode *root, SymbolTable **symtab, ErrorTable *errtab, const char *filename);

#endif /* SEMANTIC_ANALYSIS_H */
