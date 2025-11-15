#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../header/operator-afn.h"

OperatorState operator_next_state(OperatorState current, char c) {
    switch (current) {
        case OP_Q0:
            if (c == '+') return OP_Q1;
            if (c == '-') return OP_Q1;
            if (c == '*') return OP_Q1;
            if (c == '/') return OP_Q1;
            if (c == '=') return OP_Q2;
            if (c == '<') return OP_Q3;
            if (c == '>') return OP_Q3;
            return OP_Q_SINK;

        case OP_Q1:
            return OP_Q1;

        case OP_Q2:
            return OP_Q2;

        case OP_Q3:
            if (c == '=') return OP_Q2;
            if (c == '>') return OP_Q2; // for <>
            return OP_Q_SINK;

        case OP_Q_SINK:
        default:
            return OP_Q_SINK;
    }
}

int recognize_operator(const char *input) {
    OperatorState current_state = OP_Q0;
    int length = strlen(input);

    if (length == 0) return TOKEN_ERROR;

    if (length == 1) {
        char c = input[0];
        if (c == '+' || c == '-' || c == '*' || c == '/') return TOKEN_ARITHMETIC;
        if (c == '=' || c == '<' || c == '>') return TOKEN_RELATIONAL;
    }

    for (int i = 0; i < length; ++i) {
        current_state = operator_next_state(current_state, input[i]);
        if (current_state == OP_Q_SINK) return TOKEN_ERROR;
    }

    if (current_state == OP_Q1) {
        if (length == 1 && (input[0] == '+' || input[0] == '-' || input[0] == '*' || input[0] == '/'))
            return TOKEN_ARITHMETIC;
        if (length == 1 && (input[0] == '<' || input[0] == '>')) return TOKEN_RELATIONAL;
    }

    if (current_state == OP_Q2) {
        if (length == 2 && input[0] == ':' && input[1] == '=') return TOKEN_ASSIGNMENT;
        if (length == 2 && ((input[0] == '<' && input[1] == '>') || (input[0] == '<' && input[1] == '=') || (input[0] == '>' && input[1] == '='))) return TOKEN_RELATIONAL;
    }

    return TOKEN_ERROR;
}

#ifdef AFN_TEST
int main() {
    const char *tests[] = {
        ":=", "=", "<", ">", "<>", "<=", ">=", "+", "-", "*", "/"
    };
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; ++i) {
        int r = recognize_operator(tests[i]);
        printf("%s -> %d\n", tests[i], r);
    }
    return 0;
}
#endif