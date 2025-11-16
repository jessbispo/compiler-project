/** @brief Módulo responsável pela validação de tipos de dados em MiniPascal.
 *  @details Este módulo implementa um autômato finito não-determinístico (AFN) para validar tipos de dados no código-fonte MiniPascal.
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862)
 */

#ifndef DATATYPE_AUTOMATA_H
#define DATATYPE_AUTOMATA_H

/** @brief Estados do autômato para validação de tipos de dados.
 *  @property D_Q0 Estado inicial, aguardando o início do tipo de dado.
 *  @property D_Q1 Estado após a leitura de 'i'.
 *  @property D_Q2 Estado após a leitura de 'in'.
 *  @property D_Q3 Estado após a leitura de 'int'.
 *  @property D_Q4 Estado após a leitura de 'r'.
 *  @property D_Q5 Estado após a leitura de 're'.
 *  @property D_Q6 Estado após a leitura de 'rea'.
 *  @property D_Q7 Estado após a leitura de 'real'.
 *  @property D_Q8 Estado após a leitura de 'b'.
 *  @property D_Q9 Estado após a leitura de 'bo'.
 *  @property D_Q10 Estado após a leitura de 'boo'.
 *  @property D_Q11 Estado após a leitura de 'bool'.
 *  @property D_Q12 Estado após a leitura de 's'.
 *  @property D_Q13 Estado após a leitura de 'st'.
 *  @property D_Q14 Estado após a leitura de 'str'.
 *  @property D_Q15 Estado final, indicando o término do tipo de dado.
 */
typedef enum {
    D_Q0, D_Q1, D_Q2, D_Q3, D_Q4, D_Q5, D_Q6, D_Q7,
    D_Q8, D_Q9, D_Q10, D_Q11, D_Q12, D_Q13, D_Q14, D_Q15
} DatatypeState;

/** @brief Função para obter o próximo estado do autômato com base no estado atual e no caractere lido.
 *  @param current_state Estado atual do autômato.
 *  @param caracter Caractere lido da entrada.
 *  @return Próximo estado do autômato.
 */
DatatypeState datatype_next_state(DatatypeState current_state, char caracter);

/** @brief Validação de tipos de dados em MiniPascal.
 *  @param start String de entrada a ser verificada.
 *  @return 0 se a entrada for um tipo de dado válido e 1 caso contrário.
 */
int datatype_afn(const char *start);

#endif /* DATATYPE_AUTOMATA_H */

