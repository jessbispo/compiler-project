/** @brief Estrutura para representar o estado de um autômato. Cada estado possui um identificador único, um indicador de estado final e uma lista de transições para outros estados.
 * @param id Identificador único do estado.
 * @param is_final Indicador se o estado é um estado final (1 para verdadeiro, 0 para falso).
 * @param transitions Lista de ponteiros para as transições que saem deste estado.
 * @param transition_count Número de transições associadas a este estado.
 */
typedef struct State {
    int id;
    int is_final;
    struct Transition **transitions;
    int transition_count;
} State;

/** @brief Estrutura para representar uma transição entre estados em um autômato. Cada transição possui um símbolo de entrada e um ponteiro para o estado de destino.
 * @param symbol Símbolo de entrada que aciona a transição.
 * @param to Estado de destino para o qual a transição leva.
 */
typedef struct Transition {
    char symbol;
    struct State *to;
} Transition;

/** @brief Estrutura para representar um autômato finito. O autômato possui um estado inicial, uma lista de estados e o número total de estados.
 * @param initial_state Ponteiro para o estado inicial do autômato.
 * @param states Lista de ponteiros para todos os estados do autômato.
 * @param state_count Número total de estados no autômato.
 */
typedef struct Automaton {
    State *initial_state;
    State **states;
    int state_count;
} Automaton;

/** @brief Cria um novo estado com o identificador e indicador de estado final fornecidos.
 * @param id Identificador único do estado.
 * @param is_final Indicador se o estado é um estado final (1 para verdadeiro, 0 para falso).
 * @return Ponteiro para o estado recém-criado.
 */
State* create_state(int id, int is_final);

/** @brief Adiciona uma transição de um estado para outro com o símbolo de entrada fornecido.
 * @param from Ponteiro para o estado de origem da transição.
 * @param symbol Símbolo de entrada que aciona a transição.
 * @param to Ponteiro para o estado de destino da transição.
 */
void add_transition(State *from, char symbol, State *to);

/** @brief Cria um novo autômato vazio.
 * @return Ponteiro para o autômato recém-criado.
 */
Automaton* create_automaton();

/** @brief Adiciona um estado ao autômato.
 * @param automaton Ponteiro para o autômato ao qual o estado será adicionado.
 * @param state Ponteiro para o estado a ser adicionado.
 */
void add_state(Automaton *automaton, State *state);

/** @brief Define o estado inicial do autômato.
 * @param automaton Ponteiro para o autômato.
 * @param state Ponteiro para o estado que será definido como estado inicial.
 */
void set_start_state(Automaton *automaton, State *state);

/** @brief Adiciona um estado final ao autômato.
 * @param automaton Ponteiro para o autômato.
 * @param state Ponteiro para o estado que será adicionado como estado final.
 */
void add_final_state(Automaton *automaton, State *state);

/** @brief Verifica se o estado atual é um estado final do autômato.
 * @param automaton Ponteiro para o autômato.
 * @param state Ponteiro para o estado a ser verificado.
 * @return 1 se o estado for final, 0 caso contrário.
 */
int is_in_final_state(Automaton *automaton, State *state);

/** @brief Processa uma cadeia de entrada através do autômato e determina se a cadeia é aceita (termina em um estado final).
 * @param automaton Ponteiro para o autômato.
 * @param input Cadeia de entrada a ser processada.
 * @return 1 se a cadeia for aceita (termina em um estado final), 0 caso contrário.
 */
int process_input(Automaton *automaton, const char *input);

/** @brief Libera a memória alocada para o autômato e seus componentes.
 *  @param automaton Ponteiro para o autômato a ser liberado.
 *  @return 0 se a liberação for bem-sucedida, -1 em caso de erro.
 */
int free_automaton(Automaton *automaton);