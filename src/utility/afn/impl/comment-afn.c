/** @brief Implementação do autômato para validação de comentários de bloco.
 * @authors Jessica Bispo (), Vitor Alves Pereira (10410862)
 */

#include "../header/comment-afn.h"
#include <stdio.h>
#include <string.h>

/** @brief Validação de comentários de bloco utilizados em MiniPascal.
 * @param input String de entrada a ser verificada
 * @return 0 se a entrada for um comentario válido e 1 caso contrário.
 */
int comment_afn(const char *input) {
    CommentState state = C_Q0;

    for (int i = 0; input[i] != '\0'; i++) {
        char c = input[i];

        switch (state) {
            case C_Q0:
                if (c == '/') state = C_Q1;
                else state = C_Q0;
                break;

            case C_Q1:
                if (c == '*') state = C_Q2;
                else if (c == '/') state = C_Q1;
                else state = C_Q0;
                break;

            case C_Q2:
                if (c == '*') state = C_Q3;
                else state = C_Q2; 
                break;

            case C_Q3:
                if (c == '/') state = C_Q4; 
                else if (c == '*') state = C_Q3; 
                else state = C_Q2; 
                break;

            case C_Q4:
                break;
        }

        if (state == C_Q4 && input[i+1] != '\0') {
            return 1;
        }
    }

    return state == C_Q4 ? 0 : 1;
}

#ifdef AFN_TEST
int main(void) {
    char input[500];

    printf("Digite uma string: ");
    if (!fgets(input, sizeof(input), stdin)) return 1;
    input[strcspn(input, "\n")] = '\0';

    if (comment_afn(input) == 0)
        printf("Comentário válido!\n");
    else
        printf("Comentário inválido!\n");

    return 0;
}
#endif
