/** @brief Módulo responsável pela validação de delimitadores em MiniPascal.
 *  @details Este módulo implementa um autômato finito não-determinístico (AFN) para validar delimitadores no código-fonte MiniPascal.
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862)
 */

#ifndef DELIMITER_H
#define DELIMITER_H

/** @brief Estados do autômato para validação de delimitadores.
 *  @property DEL_Q1 Estado inicial, aguardando o início do delimitador.
 *  @property DEL_Q2 Estado após a leitura de '('.
 *  @property DEL_Q3 Estado após a leitura de ')'.
 *  @property DEL_Q4 Estado após a leitura de '['.
 *  @property DEL_Q5 Estado após a leitura de ']'.
 *  @property DEL_Q6 Estado após a leitura de '{'.
 *  @property DEL_Q7 Estado após a leitura de '}'.
 *  @property DEL_Q8 Estado após a leitura de ';'.
 *  @property DEL_Q9 Estado após a leitura de ','.
 *  @property DEL_Q10 Estado após a leitura de '.'.
 *  @property DEL_Q11 Estado final, indicando o término do delimitador.
 */
typedef enum {
    DEL_Q1, DEL_Q2, DEL_Q3, DEL_Q4, DEL_Q5, DEL_Q6, DEL_Q7, DEL_Q8, DEL_Q9, DEL_Q10, DEL_Q11
} DelimiterState;

/** @brief Função para obter o próximo estado do autômato com base no estado atual e no caractere lido.
 *  @param current_state Estado atual do autômato.
 *  @param character Caractere lido da entrada.
 *  @return Próximo estado do autômato.
 */
DelimiterState delimiter_next_state(DelimiterState current_state, char character);
int delimiter_afn(const char *input);

#endif /* DELIMITER_H */
