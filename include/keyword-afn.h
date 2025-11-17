/** @brief Módulo responsável pela validação de palavras-chave em MiniPascal.
 *  @details Este módulo implementa um autômato finito não-determinístico (AFN) para validar palavras-chave no código-fonte MiniPascal.
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862)
 */

#ifndef KEYWORD_AFN_H
#define KEYWORD_AFN_H

/** @brief Estrutura para representar o estado do autômato de palavras-chave.
 *  @property state Estado atual do autômato.
 *  @property is_final Indica se o estado atual é um estado final.
 */
typedef struct {
    int state;
    int is_final;
} KeywordState;

/** @brief Verifica se um caractere é válido para palavras-chave.
 *  @param c Caractere a ser verificado.
 *  @return 1 se o caractere for válido, 0 caso contrário.
 */
int is_keyword_char(char c);

/** @brief Verifica se a string de entrada é uma palavra-chave válida.
 *  @param input String de entrada a ser verificada.
 *  @return 1 se a entrada for uma palavra-chave válida, 0 caso contrário.
 */
int is_keyword(const char* input);

/** @brief Função de transição do autômato de palavras-chave.
 *  @param current Estado atual do autômato.
 *  @param c Caractere lido da entrada.
 *  @return Próximo estado do autômato.
 */
KeywordState transition(KeywordState current, char c);

#endif
