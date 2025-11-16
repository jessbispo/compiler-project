#ifndef BEAUTIFUL_PRINT_H
#define BEAUTIFUL_PRINT_H

#include <stdio.h>
#include "symbolTable.h"
#include "errorHandler.h"
/* forward declare ASTNode to avoid include cycles */
typedef struct ASTNode ASTNode;

#define RESET        "\033[0m"

#define BOLD_CYAN    "\033[1;36m"
#define BOLD_YELLOW  "\033[1;33m"
#define BOLD_GREEN   "\033[1;32m"
#define BOLD_MAGENTA "\033[1;35m"
#define BOLD_BLUE    "\033[1;34m"
#define BOLD_RED     "\033[1;31m"

#define WHITE        "\033[0;37m"
#define CYAN         "\033[0;36m"
#define GREY         "\033[0;90m"

/** @brief Impressão do título estilizado do projeto MiniPascal Compiler.
 *  @return void
 */
void printTitle();

/** @brief Imprime a tabela de símbolos
 * 
 */
void printSymbolTable(SymbolTable **hash_table);

void printErrorTable(ErrorTable **error_table);

void printMessage(char *message, int messageType);

void printDivisor();

void receiveEntry(char *message, int entryType);

void printSubtitle(char *message);

#endif