#ifndef DATATYPE_AUTOMATA_H
#define DATATYPE_AUTOMATA_H

typedef enum {
    D_Q0, D_Q1, D_Q2, D_Q3, D_Q4, D_Q5, D_Q6, D_Q7,
    D_Q8, D_Q9, D_Q10, D_Q11, D_Q12, D_Q13, D_Q14, D_Q15
} DatatypeState;

DatatypeState datatype_next_state(DatatypeState current_state, char caracter);

int datatype_afn(const char *start);

#endif /* DATATYPE_AUTOMATA_H */

