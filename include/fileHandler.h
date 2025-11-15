/** @brief File Handler é um módulo responsável pelo gerenciamento e manipulação de arquivos.
 *  @headerfile fileHnadler.h 
 *  @authors Jessica Bispo, Vitor Alves Pereira
 *  @date 2025-11-11
 */

#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <stdio.h>

/** @brief Recebe o arquivo com base em um caminho fornecido como argumento.
 *  @param file Ponteiro para o arquivo a ser aberto.
 *  @param filepath Caminho do arquivo a ser aberto.
 *  @param mode Modo de abertura do arquivo (r, w, a, rb, wb, ab).
 *  @return Retorna 0 se o arquivo foi aberto com sucesso, ou um código de erro caso contrário.
 */
int openFile(FILE **file, const char *filepath, char *mode);

/** @brief Valida o caminho do arquivo fornecido pelo usuário.
 *  @param filepath Caminho do arquivo a ser validado
 *  @return Retorna 0 se o caminho for válido, ou 1 caso contrário.
 */
int validateFilePath(const char *filepath);

/** @brief Escreve o conteúdo fornecido em um arquivo no caminho especificado.
 *  @param filepath Caminho do arquivo onde o conteúdo será escrito.
 *  @param content Conteúdo a ser escrito no arquivo.
 *  @return Retorna 0 se a escrita foi bem-sucedida, ou um código de erro caso contrário.
 */
int writeOnFile(const char *filepath, const char *content);

/** @brief Lê o conteúdo de um arquivo no caminho especificado.
 *  @param filepath Caminho do arquivo a ser lido.
 *  @param contentBuffer Ponteiro para o buffer onde o conteúdo lido será armazenado.
 *  @return Retorna o número de bytes lidos, ou um código de erro caso contrário.
 */
int readFileContent(const char *filepath, char **contentBuffer);

/** @brief Cria um novo arquivo no caminho especificado.
 *  @param filepath Caminho do arquivo a ser lido.
 *  @return Retorna 0 se o arquivo foi criado com sucesso, ou um código de erro caso contrário.
 */
int createFile(const char *filepath);

/** @brief Fecha o arquivo fornecido.
 *  @param file Ponteiro para o arquivo a ser fechado.
 *  @return Retorna 0 se o arquivo foi fechado com sucesso, ou um código de erro caso contrário.
 */
int closeFile(FILE *file);

#endif