/** @brief Implementação do autômato finito não-determinístico para validação de declarações de delimitadores e tipos em MiniPascal.
 *  @details A documentação completa de cada função está disponível no arquivo de cabeçalho correspondente.
 *  @headerfile delimiter-afn.h
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862)
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../../../include/delimiter-afn.h"
#include "../../../include/datatype-afn.h"

DelimiterState delimiter_next_state(DelimiterState current_state, char character) {
    switch (current_state) {
        case DEL_Q1:
            if (isalpha(character)) return DEL_Q2;
            if (character == 'e') return DEL_Q7;
            return DEL_Q6;

        case DEL_Q2:
            if (isalnum(character)) return DEL_Q2;
            if (character == ',') return DEL_Q3;
            if (character == ':') return DEL_Q4;
            return DEL_Q6;

        case DEL_Q3:
            if (isalpha(character)) return DEL_Q2;
            return DEL_Q6;

        case DEL_Q4:
            if (isalpha(character)) return DEL_Q5;
            return DEL_Q6;

        case DEL_Q5:
            if (isalnum(character)) return DEL_Q5;
            if (character == ';') return DEL_Q8;
            return DEL_Q6;

        case DEL_Q7:
            if (character == 'n') return DEL_Q9;
            return DEL_Q6;

        case DEL_Q9:
            if (character == 'd') return DEL_Q10;
            return DEL_Q6;

        case DEL_Q10:
            if (character == '.') return DEL_Q11;
            return DEL_Q6;

        case DEL_Q8:
        case DEL_Q11:
            return current_state;

        case DEL_Q6:
            return DEL_Q6;

        default:
            return DEL_Q6;
    }
}

int check_datatype(const char *input, int start, int end) {
    char type[32];
    int j = 0;
    
    for (int i = start; i < end && j < 31; i++) {
        type[j++] = input[i];
    }
    type[j] = '\0';
    
    return datatype_afn(type);
}

int delimiter_afn(const char *input) {
    DelimiterState current_state = DEL_Q1;
    int length = strlen(input);
    int i = 0;
    int type_start = -1;

    while (i < length) {
        if (current_state == DEL_Q5 && type_start == -1) {
            type_start = i;
        }
        
    current_state = delimiter_next_state(current_state, input[i]);
        
        if (current_state == DEL_Q6) {
            return 0;
        }

        if (current_state == DEL_Q8 && type_start != -1) {
            if (!check_datatype(input, type_start, i)) {
                return 0;
            }
        }
        
        i++;
    }

    return (current_state == DEL_Q8 || current_state == DEL_Q11);
}

#ifdef AFN_TEST
int main() {
    const char *tests[] = {
        "var1, var2: integer;",
        "abc: boolean;",
        "end.",
        "abc: string;",
        "var1, var2: bool;",
        "abc, def: integer",
        "end",
        "abc:",
        "abc; integer;",
        "var1, : integer;",
        ": integer;",
        "var1,var2: boolean;"
    };
    
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    
    printf("--- Testing Delimiter and Type Declaration Automaton ---\n");
    for (int i = 0; i < num_tests; i++) {
        int result = delimiter_afn(tests[i]);
        printf("Input: \"%s\" -> Result: %s\n", tests[i], result ? "ACCEPTED" : "REJECTED");
    }

    return 0;
}
#endif
