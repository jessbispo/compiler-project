#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../header/datatype-afn.h"

DatatypeState datatype_next_state(DatatypeState current_state, char caracter) {
    switch (current_state) {
        case D_Q0:
            if (caracter == 'i') return D_Q1;
            if (caracter == 'b') return D_Q8;
            return D_Q14;
        
        case D_Q1:
            if (caracter == 'n') return D_Q2;
            return D_Q15; 
        case D_Q2:
            if (caracter == 't') return D_Q3;
            return D_Q15; 
        case D_Q3:
            if (caracter == 'e') return D_Q4;
            return D_Q15; 
        case D_Q4:
            if (caracter == 'g') return D_Q5;
            return D_Q15; 
        case D_Q5:
            if (caracter == 'e') return D_Q6;
            return D_Q15; 
        case D_Q6:
            if (caracter == 'r') return D_Q7; 
            return D_Q15; 
        
        case D_Q8:
            if (caracter == 'o') return D_Q9;
            return D_Q14; 
        case D_Q9:
            if (caracter == 'o') return D_Q10;
            return D_Q14; 
        case D_Q10:
            if (caracter == 'l') return D_Q11;
            return D_Q14; 
        case D_Q11:
            if (caracter == 'e') return D_Q12;
            return D_Q14; 
        case D_Q12:
            if (caracter == 'a') return D_Q13;
            return D_Q14; 
        case D_Q13:
            if (caracter == 'n') return D_Q7; 
            return D_Q14; 

        case D_Q7:
        case D_Q14: 
        case D_Q15: 
            return current_state; 
        
        default:
            return D_Q14; 
    }
}

int datatype_afn(const char *start) {
    DatatypeState current_state = D_Q0;
    int length = strlen(start);
    int i = 0;

    while (i < length) {
    current_state = datatype_next_state(current_state, start[i]);
        if (current_state == D_Q14 || current_state == D_Q15) {
            return 0;
        }
        
        i++;
    }

    if (current_state == D_Q7) {
        return 1;
    }

    return 0;
}

#ifdef AFN_TEST
int main() {
    const char *testes[] = {
        "INTEGER",
        "iNtEgEr", 
        "BOOLEAN",
        "Boolean", 
        "INT",     
        "BOOLEANO", 
        "OUTRO"    
    };
    
    int num_testes = sizeof(testes) / sizeof(testes[0]);
    
    printf("--- Teste do Automato Finito para Palavras-Chave ---\n");
    
    for (int i = 0; i < num_testes; i++) {
        int resultado = is_keyword(testes[i]);
        printf("Palavra: \"%s\" -> Resultado: %s\n", testes[i], resultado ? "**ACEITA**" : "REJEITA");
    }

    return 0;
}
#endif