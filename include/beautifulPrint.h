/** @brief BeautifulPrint é o módulo responsável por centralizar todas as funcionalidade de impressão realizadas ao longo do programa.
 *  @details Possui impressões estilizadas com cores e outras impressões simples para demonstração no terminal.
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862)
 */

#ifndef BEAUTIFUL_PRINT_H
#define BEAUTIFUL_PRINT_H

#include <stdio.h>
#include "symbolTable.h"
#include "errorHandler.h"

// Configuração de Cores de RESET até GREY
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

/**/
typedef struct ASTNode ASTNode;

/** @brief Impressão estilizada para o título, subtítulo e fases do projeto de compilador.
 *  @details O estilo utilizado para o título segue uma tipografia Heavy Block estilizado com bold cyan.
 *  @return void
 */
void printTitle();

/** @brief Impressão da tabela de símbolos.
 *  @param hash_table Tabela de símbolos a ser impressa.
 *  @return void
 */
void printSymbolTable(SymbolTable **hash_table);

/** @brief Impressão da tabela de erros.
 *  @param error_table Tabela de erros a ser impressa.
 *  @return void
 */
void printErrorTable(ErrorTable **error_table);

/** @brief Impressão de mensagens estilizadas conforme o tipo.
 *  @param message Mensagem a ser impressa.
 *  @param messageType Tipo da mensagem (1: Info, 2: Warning, 3: Error).
 *  @return void
 */
void printMessage(char *message, int messageType);

/** @brief Impressão do divisor de conteúdo entre as fases do compilador impressas na função principal.
 *  @return void
 */
void printDivisor();

/** @brief Recebe uma entrada do usuário com uma mensagem estilizada.
 *  @param message Mensagem a ser exibida para o usuário.
 *  @param entryType Tipo de entrada (1: Input).
 *  @return void
 */
void receiveEntry(char *message, int entryType);

/** @brief Impressão de subtítulos estilizados.
 *  @param message Mensagem do subtítulo a ser impressa.
 *  @return void
 */
void printSubtitle(char *message);

#endif