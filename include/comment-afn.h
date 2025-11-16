/** @brief Módulo responsável pela validação de comentários de bloco em MiniPascal.
 *  @details Este autômato possui quatro estados: C_Q0 (estado inicial), C_Q1 (após ler '/'), C_Q2 (dentro do comentário), C_Q3 (após ler '*'), e C_Q4 (estado de aceitação após ler '*'e '/')
 *  @details O autômato aceita strings que começam com / e * e terminam com * e /, permitindo qualquer caractere entre eles, incluindo quebras de linha.
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (104108
 */

#ifndef COMMENTS_AUTOMATA_H
#define COMMENTS_AUTOMATA_H

/** @brief Estados do autômato para validação de comentários de bloco.
 *  @property C_Q0 Estado inicial, aguardando o início do comentário.
 *  @property C_Q1 Estado após a leitura de '/'.
 *  @property C_Q2 Estado dentro do comentário após a leitura de '/*'.
 *  @property C_Q3 Estado após a leitura de '*' dentro do comentário.
 *  @property C_Q4 Estado final, indicando o término do comentário.
 */
typedef enum {
    C_Q0, C_Q1, C_Q2, C_Q3, C_Q4
} CommentState;

/** @brief Validação de comentários de bloco em MiniPascal.
 *  @param input String de entrada a ser verificada.
 *  @return 0 se a entrada for um comentário válido e 1 caso contrário.
 */
int comment_afn(const char *input);

#endif /* COMMENTS_AUTOMATA_H */

