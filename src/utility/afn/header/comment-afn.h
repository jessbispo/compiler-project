/** @brief Interface para Verificação de Comentários de Bloco em MiniPascal
 * @authors Jessica Bispo (), Vitor Alves Pereira (10410862)
 */

#ifndef COMMENTS_AUTOMATA_H
#define COMMENTS_AUTOMATA_H

/** @brief Estrutura do autômato finito para validação dos comentários de bloco
 */
typedef enum {
    C_Q0, C_Q1, C_Q2, C_Q3, C_Q4
} CommentState;

/** @brief Validação de comentários de bloco utilizados em MiniPascal.
 * @param input String de entrada a ser verificada
 * @return 0 se a entrada for um comentario válido e 1 caso contrário.
 */
int comment_afn(const char *input);

#endif /* COMMENTS_AUTOMATA_H */

