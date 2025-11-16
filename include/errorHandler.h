/** @brief ErrorHandler é o módulo responsável por gerenciar a tabela de erros do compilador.
 *  @details É utilizado para gerenciar e armazenar erros léxicos, sintáticos e semânticos encontrados durante o processo de compilação.
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862)
 */

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <time.h>

/** @brief Enumeração dos tipos de erro que a tabela de erros pode registrar.
 *  @property LEXICAL_ERROR Erro léxico
 *  @property SYNTACTIC_ERROR Erro sintático
 *  @property SEMANTIC_ERROR Erro semântico
 *  @property RUNTIME_ERROR Erro em tempo de execução
 */
typedef enum {
    LEXICAL_ERROR,
    SYNTACTIC_ERROR,
    SEMANTIC_ERROR,
    RUNTIME_ERROR
} ErrorType;

/** @brief Estrutura para representar um erro a ser categorizado e armazenado na tabela de erros.
 *  @property name Nome/identificador do erro
 *  @property description Descrição detalhada do erro
 *  @property timestamp Timestamp indicando quando o erro foi registrado
 *  @property type Tipo do erro (léxico, sintático, semântico, etc.)
 *  @property line Linha onde o erro ocorreu
 *  @property filename Nome do arquivo onde o erro ocorreu
 *  @property snippet Trecho de código relacionado ao erro
 *  @property next Ponteiro para o próximo erro na lista (para tratamento de colisões
 */
typedef struct Error {
    char* name;
    char* description;
    char* message;
    time_t timestamp;
    ErrorType type;
    int line;
    char* filename;
    char* snippet;
    struct Error* next;
} Error;

/** @brief Estrutura para representar a tabela de erros usando uma tabela hash.
 *  @property size Tamanho da tabela hash
 *  @property count Número atual de erros armazenados na tabela
 *  @property errors Array de ponteiros para listas encadeadas de erros (para tratamento de colisões)
 */
typedef struct ErrorTable {
    unsigned int size;
    unsigned int line;
    unsigned int count;
    char *message;
    char *type;
    char *filename;
    struct ErrorTable* next;
    Error** errors;
} ErrorTable;

/** @brief Inicializa uma tabela de erros
 *  @param size Tamanho inicial da tabela de erros
 *  @return ErrorTable* Ponteiro para a tabela de erros criada, ou NULL em caso de falha
 */
ErrorTable* createErrorTable(unsigned int size);

/** @brief Função hash para mapear o nome do erro para um índice na tabela
 *  @param name Nome do erro
 *  @param tableSize Tamanho da tabela de erros
 *  @return unsigned int Índice calculado na tabela
 */
unsigned int hashError(const char* name, unsigned int tableSize);

/** @brief Insere um novo erro na tabela de erros 
 *  @param table A tabela de erros onde o erro será inserido
 *  @param name Nome do erro
 *  @param description Descrição detalhada do erro
 *  @param type Tipo do erro (léxico, sintático, semântico, etc.)
 *  @param line Linha onde o erro ocorreu
 *  @param filename Nome do arquivo onde o erro ocorreu
 *  @return int 0 em caso de sucesso, -1 em caso de falha
 */
int insertError(ErrorTable* table, const char* name, const char* description, ErrorType type, int line, const char* filename);

/** @brief Procura um erro na tabela pelo seu nome
 *  @param table A tabela de erros onde a busca será realizada
 *  @param name O nome do erro a ser buscado
 *  @return Error* Ponteiro para o erro encontrado, ou NULL se não encontrado
 */
Error* findError(ErrorTable* table, const char* name);

/** @brief Remove um erro da tabela pelo seu nome
 *  @param table A tabela de erros de onde o erro será removido
 *  @param name O nome do erro a ser removido
 *  @return int 0 em caso de sucesso, -1 se o erro não for encontrado
 */
int removeError(ErrorTable* table, const char* name);

/** @brief Destrói a tabela de erros, liberando toda a memória alocada
 *  @param table A tabela de erros a ser destruída
 */
void destroyErrorTable(ErrorTable* table);

/** @brief Imprime o conteúdo da tabela de erros para depuração
 *  @param table A tabela de erros a ser impressa
 */
void printErrorTable(ErrorTable** table);

#endif /* ERROR_HANDLER_H */
