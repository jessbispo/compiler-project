#include "automata.h"

/**
 * @brief Estrutura para representar a posição de um token no código-fonte. 
 */
typedef struct Position {
    int line;
    int column;
} Position;

/**
 * @brief Estrutura para representar um token identificado na análise léxica.
 */
typedef struct Token {
    char *lexeme;
    Position position;
} Token;

/**
 * @brief Estrutura para representar uma tabela de tokens.
 */
typedef struct TokensTable {
    Token **tokens;
    int count;
} TokensTable;

/** @brief Cria um novo token com o lexema, linha e coluna fornecidos.
 *  @param lexeme O lexema do token.
 *  @param line A linha onde o token foi encontrado.
 *  @param column A coluna onde o token foi encontrado.
 *  @return Um ponteiro para o token criado.
 */
Token* create_token(const char *lexeme, int line, int column);

/** @brief Adiciona um token à tabela de tokens.
 *  @param table A tabela de tokens onde o token será adicionado.
 *  @param token O token a ser adicionado.
 *  @return void
 */
void add_token_to_table(TokensTable *table, Token *token);

/** @brief Libera a memória alocada para um token.
 *  @param token O token a ser liberado.
 *  @return void
 */
void free_token(Token *token);

/** @brief Libera a memória alocada para uma lista de tokens.
 *  @param tokens A lista de tokens a ser liberada.
 *  @param count O número de tokens na lista.
 *  @return void
 */
void free_all_tokens(Token **tokens, int count);

/** @brief Realiza a análise léxica no código-fonte fornecido.
 *  @param source_code O código-fonte a ser analisado.
 *  @param tokens Um ponteiro para um array de tokens onde os tokens identificados serão armazenados.
 *  @param token_count Um ponteiro para um inteiro onde o número de tokens identificados será armazenado.
 *  @return void
 */
void lexical_analysis(const char *source_code, Token ***tokens, int *token_count);

/** @brief Analisa o conteúdo do arquivo e realiza a análise léxica.
 *  @param file_content O conteúdo do arquivo a ser analisado.
 *  @param file_size O tamanho do conteúdo do arquivo.
 *  @return void
 */
void analyze_file_content(char *file_content, int file_size, TokensTable *table);

// ---- Delimitadores ----

/** @brief Autômato para reconhecer comentários no código-fonte.
 *  @param lexeme O lexema atual sendo analisado.
 *  @param index Um ponteiro para o índice atual no lexema.
 *  @param line Um ponteiro para a linha atual no código-fonte.
 *  @param column Um ponteiro para a coluna atual no código-fonte.
 *  @param tokens Um ponteiro para um array de tokens onde os tokens identificados serão armazenados.
 *  @param token_count Um ponteiro para um inteiro onde o número de tokens identificados será armazenado.
 *  @return Token O token de comentário criado.
 */
Token comments_automaton(char *lexeme, int *index, int *line, int *column, Token ***tokens, int *token_count);

/** @brief Autômato para reconhecer delimitadores no código-fonte.
 *  @param lexeme O lexema atual sendo analisado.
 *  @param index Um ponteiro para o índice atual no lexema.
 *  @param line Um ponteiro para a linha atual no código-fonte.
 *  @param column Um ponteiro para a coluna atual no código-fonte.
 *  @param tokens Um ponteiro para um array de tokens onde os tokens identificados serão armazenados.
 *  @param token_count Um ponteiro para um inteiro onde o número de tokens identificados será armazenado.
 *  @return void
 */
void delimiters_automaton(char *lexeme, int *index, int *line, int *column, Token ***tokens, int *token_count);

/** @brief Autômato para reconhecer operadores no código-fonte.
 *  @param lexeme O lexema atual sendo analisado.
 *  @param index Um ponteiro para o índice atual no lexema.
 *  @param line Um ponteiro para a linha atual no código-fonte.
 *  @param column Um ponteiro para a coluna atual no código-fonte.
 *  @param tokens Um ponteiro para um array de tokens onde os tokens identificados serão armazenados.
 *  @param token_count Um ponteiro para um inteiro onde o número de tokens identificados será armazenado.
 *  @return void
 */
void operators_automaton(char *lexeme, int *index, int *line, int *column, Token ***tokens, int *token_count);

/** @brief Autômato para reconhecer palavras reservadas no código-fonte.
 *  @param lexeme O lexema atual sendo analisado.
 *  @param index Um ponteiro para o índice atual no lexema.
 *  @param line Um ponteiro para a linha atual no código-fonte.
 *  @param column Um ponteiro para a coluna atual no código-fonte.
 *  @param tokens Um ponteiro para um array de tokens onde os tokens identificados serão armazenados.
 *  @param token_count Um ponteiro para um inteiro onde o número de tokens identificados será armazenado.
 *  @return void
 */
void reserved_words_automaton(char *lexeme, int *index, int *line, int *column, Token ***tokens, int *token_count);

/** @brief Autômato para reconhecer identificadores no código-fonte.
 *  @param lexeme O lexema atual sendo analisado.
 *  @param index Um ponteiro para o índice atual no lexema.
 *  @param line Um ponteiro para a linha atual no código-fonte.
 *  @param column Um ponteiro para a coluna atual no código-fonte.
 *  @param tokens Um ponteiro para um array de tokens onde os tokens identificados serão armazenados.
 *  @param token_count Um ponteiro para um inteiro onde o número de tokens identificados será armazenado.
 *  @return void
 */
void identifiers_automaton(char *lexeme, int *index, int *line, int *column, Token ***tokens, int *token_count);

/** @brief Autômato para reconhecer números no código-fonte.
 *  @param lexeme O lexema atual sendo analisado.
 *  @param index Um ponteiro para o índice atual no lexema.
 *  @param line Um ponteiro para a linha atual no código-fonte.
 *  @param column Um ponteiro para a coluna atual no código-fonte.
 *  @param tokens Um ponteiro para um array de tokens onde os tokens identificados serão armazenados.
 *  @param token_count Um ponteiro para um inteiro onde o número de tokens identificados será armazenado.
 *  @return void
 */
void numbers_automaton(char *lexeme, int *index, int *line, int *column, Token ***tokens, int *token_count);

/** @brief Autômato para reconhecer espaços em branco no código-fonte.
 *  @param lexeme O lexema atual sendo analisado.
 *  @param index Um ponteiro para o índice atual no lexema.
 *  @param line Um ponteiro para a linha atual no código-fonte.
 *  @param column Um ponteiro para a coluna atual no código-fonte.
 *  @return void
 */
void whitespace_automaton(char *lexeme, int *index, int *line, int *column);
