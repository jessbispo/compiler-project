/** @brief Autômato finito não-determinístico para validação de comentários de bloco em MiniPascal.
 *  @details A documentação completa de cada função está disponível no arquivo de cabeçalho correspondente.
 *  @headerfile comment-afn.h
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862) 
 */

#include "../../../include/comment-afn.h"
#include <stdio.h>
#include <string.h>

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
