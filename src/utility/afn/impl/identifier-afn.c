#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "../header/identifier-afn.h"

int identifier_afn(const char *input) {
    IdentifierState state = ID_Q0;
    int i = 0;

    while (input[i] != '\0') {
        char c = input[i];

        switch (state) {
            case ID_Q0:
                if (isalpha(c) || c == '_') {
                    state = ID_Q1;
                } else if (isdigit(c)) {
                    state = ID_Q2; 
                } else {
                    state = ID_Q2;
                }
                break;

            case ID_Q1:
                if (isalnum(c) || c == '_') {
                    state = ID_Q1;
                } else {
                    state = ID_Q2;
                }
                break;

            case ID_Q2:
                return false; 
        }

        i++;
    }
    /* Return 1 when accepted (state ID_Q1), 0 otherwise */
    return (state == ID_Q1) ? 1 : 0;
}

#ifdef AFN_TEST
int main() {
    char input[100];

    printf("Digite um identificador: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    if (identifier_afn(input))
        printf("Identificador válido!\n");
    else
        printf("Identificador inválido!\n");

    return 0;
}
#endif
