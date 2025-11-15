#include <stdio.h>

#ifndef IDENTIFIERS_AUTOMATA_H

typedef enum {
    ID_Q0, ID_Q1, ID_Q2
} IdentifierState;

int identifier_afn(const char *input);

#endif /* IDENTIFIERS_AUTOMATA_H */