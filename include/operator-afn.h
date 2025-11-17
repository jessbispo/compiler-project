/** @brief Módulo responsável pela validação de operadores em MiniPascal.
 *  @details Este módulo implementa um autômato finito não-determinístico (AFN) para validar operadores no código-fonte MiniPascal.
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862)
 */

#ifndef OPERATOR_AFN_H
#define OPERATOR_AFN_H

#define TOKEN_ARITHMETIC    1
#define TOKEN_RELATIONAL    2
#define TOKEN_ASSIGNMENT    3
#define TOKEN_LOGICAL       4
#define TOKEN_ERROR         0

/** @brief Estados do autômato para validação de operadores.
 *  @property OP_Q0 Estado inicial, aguardando o início do operador.
 *  @property OP_Q1 Estado após a leitura de '+' ou '-'.
 *  @property OP_Q2 Estado após a leitura de '*' ou '/'.
 *  @property OP_Q3 Estado após a leitura de '=' ou '!' ou '<' ou '>'.
 *  @property OP_Q_SINK Estado de sumidouro, indicando erro na leitura do operador.
 */
typedef enum {
    OP_Q0,
    OP_Q1, OP_Q2, OP_Q3,
    OP_Q_SINK
} OperatorState;

/** @brief Função para obter o próximo estado do autômato com base no estado atual e no caractere lido.
 *  @param current Estado atual do autômato.
 *  @param c Caractere lido da entrada.
 *  @return Próximo estado do autômato.
 */
OperatorState operator_next_state(OperatorState current, char c);

/** @brief Função para reconhecer operadores em MiniPascal.
 *  @param input String de entrada a ser verificada.
 *  @return Tipo do operador reconhecido (TOKEN_ARITHMETIC, TOKEN_RELATIONAL, TOKEN_ASSIGNMENT, TOKEN_LOGICAL) ou TOKEN_ERROR em caso de erro.
 */
int recognize_operator(const char *input);

#endif /* OPERATOR_AFN_H */