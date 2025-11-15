
#define TOKEN_ARITHMETIC    1
#define TOKEN_RELATIONAL    2
#define TOKEN_ASSIGNMENT    3
#define TOKEN_LOGICAL       4
#define TOKEN_ERROR         0

typedef enum {
    OP_Q0,
    OP_Q1, OP_Q2, OP_Q3,
    OP_Q_SINK
} OperatorState;

OperatorState operator_next_state(OperatorState current, char c);

int recognize_operator(const char *input);