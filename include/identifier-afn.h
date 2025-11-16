/** @brief Módulo responsável pela validação de identificadores em MiniPascal.
 *  @details Este módulo implementa um autômato finito não-determinístico (AFN) para validar identificadores no código-fonte MiniPascal.
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862)
 */

#ifndef IDENTIFIERS_AUTOMATA_H

/** @brief Estados do autômato para validação de identificadores.
 *  @property ID_Q0 Estado inicial, aguardando o início do identificador.
 *  @property ID_Q1 Estado após a leitura de uma letra (a-z, A-Z).
 *  @property ID_Q2 Estado final após a leitura de letras ou dígitos subsequentes (a-z, A-Z, 0-9).
 */
typedef enum {
    ID_Q0, ID_Q1, ID_Q2
} IdentifierState;

/** @brief Função para obter o próximo estado do autômato com base no estado atual e no caractere lido.
 *  @param current_state Estado atual do autômato.
 *  @param character Caractere lido da entrada.
 *  @return Próximo estado do autômato.
 */
int identifier_afn(const char *input);

#endif /* IDENTIFIERS_AUTOMATA_H */