#include "lexical-analysis.h"

/** 
 * @brief Função para imprimir uma mensagem de boas-vindas ao usuário.
 * @return void
 */
void greetings();

/** 
 * @brief Função para imprimir uma mensagem de erro genérica.
 * @return void
 */
void error_message();

/**
 * @brief Função para imprimir uma mensagem de sucesso genérica.
 * @param message_status Um código indicando o tipo de mensagem de sucesso a ser exibida.
 * - 1: Indica que o arquivo foi lido com sucesso.
 * @return void
 */
void success_message(int message_status);

/**
 * @brief Função para exibir o conteúdo de um arquivo.
 * @param filepath O caminho do arquivo.
 * @param content Ponteiro para o conteúdo do arquivo.
 * @param size Tamanho do conteúdo do arquivo em bytes.
 * @return void
 */
void show_file_content(char *filepath, char *content, size_t size);

/** 
 * @brief Função para imprimir uma mensagem indicando o início da análise léxica.
 * @return void
 */
void lexical_analysis_message();

/** 
 * @brief Função para imprimir a tabela de tokens identificados durante a análise léxica.
 * @return void
 */
void print_token_table(TokensTable *table);
