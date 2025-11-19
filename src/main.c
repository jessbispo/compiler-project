/** @name MiniPascal Compiler Project
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862) 
 *  @brief Função Central do Projeto de Compilador em MiniPascal em C
 *  @details Recebe um arquivo de entrada assim que o arquivo foi compilado e executado, ou solicita o caminho do arquivo ao usuário.
 *  @details Utiliza o comando gcc -Wall -Wno-unused-result -g -Og src/main.c -o compiler para compilar o código.
 *  @implements Análise Léxica, Análise Sintática, Análise Semântica e Geração de Código Intermediário.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>

// ================================================================================================================================================
// TABELA DE SÍMBOLOS
// ================================================================================================================================================

#define HASH_TABLE_SIZE 211

/** @brief Estrutura da Tabela de Símbolos
 *  @property name Nome do Símbolo
 *  @property category Categoria do Símbolo
 *  @property type Tipo do Símbolo
 *  @property attributes Atributos do Símbolo
 *  @property scope Escopo do Símbolo
 *  @property line Linha onde o Símbolo está na Tabela de Símbolos
 */
typedef struct SymbolTable {
    char *name;
    char *category;
    char *type;
    char *scope;
    char *address;       
    char *parameters;    
    char *literal_value; 
    char *declared_in;   
    char *visibility;    
    int size;            
    int line;            
    struct SymbolTable *next;
} SymbolTable;

/** @brief Função para calcular o índice hash de uma string
 *  @param str String para calcular o hash
 *  @return Índice hash calculado
 */
unsigned int hashFunction(const char *str);

/** @brief Função para inicializar a tabela de símbolos
 *  @return Ponteiro para array de ponteiros SymbolTable inicializado
 */
SymbolTable** initSymbolTable();

/** @brief Função para criar uma nova entrada na Tabela de Símbolos
 *  @param name Nome do Símbolo
 *  @param category Categoria do Símbolo
 *  @param type Tipo do Símbolo
 *  @param attributes Atributos do Símbolo
 *  @param scope Escopo do Símbolo
 *  @param line Linha onde o Símbolo está na Tabela de Símbolos
 *  @return Ponteiro para a nova entrada da Tabela de Símbolos
 */
SymbolTable* createSymbol(const char *name, const char *category, const char *type,
                           const char *scope, const char *address, int size,
                           const char *parameters, const char *literal_value,
                           const char *declared_in, const char *visibility, int line);

/** @brief Insere um símbolo na tabela hash
 *  @param hash_table Ponteiro para a tabela hash
 *  @param symbol Ponteiro para o símbolo a ser inserido
 *  @return 1 se inserido com sucesso, 0 caso contrário
 */
int insertSymbol(SymbolTable **hash_table, SymbolTable *symbol);

/** @brief Busca por um Símbolo na Tabela de Símbolos
 *  @details Utiliza a Estratégia de Função Hash para Localizar o Símbolo
 *  @param hash_table Ponteiro para a Tabela Hash de Símbolos
 *  @param name Nome do Símbolo a ser buscado
 *  @return Ponteiro para o Símbolo encontrado ou NULL se não encontrado
 */
SymbolTable* searchSymbol(SymbolTable **hash_table, const char *name);

/** @brief Libera toda a memória da tabela de símbolos
 *  @param hash_table Ponteiro para a Tabela Hash de Símbolos
 */
void destroySymbolTable(SymbolTable **hash_table);

/** @brief Limpa todos os símbolos da tabela mantendo a alocação
 *  @param hash_table Ponteiro para a Tabela Hash de Símbolos
 */
void clearSymbolTable(SymbolTable **hash_table);

/** @brief Gera uma representação em string da tabela de símbolos
 *  @param hash_table Ponteiro para a Tabela Hash de Símbolos
 *  @return String representando a Tabela de Símbolos
 */
char* getSymbolTable(SymbolTable **hash_table);

// ================================================================================================================================================
// TABELA DE ERROS
// ================================================================================================================================================

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

// ================================================================================================================================================
// MÓDULO BEAUTIFUL PRINT
// ================================================================================================================================================

/** @brief BeautifulPrint é o módulo responsável por centralizar todas as funcionalidade de impressão realizadas ao longo do programa.
 *  @details Possui impressões estilizadas com cores e outras impressões simples para demonstração no terminal.
 */

// Configuração de Cores de RESET até GREY
#define RESET        "\033[0m"

#define BOLD_CYAN    "\033[1;36m"
#define BOLD_YELLOW  "\033[1;33m"
#define BOLD_GREEN   "\033[1;32m"
#define BOLD_MAGENTA "\033[1;35m"
#define BOLD_BLUE    "\033[1;34m"
#define BOLD_RED     "\033[1;31m"

#define WHITE        "\033[0;37m"
#define CYAN         "\033[0;36m"
#define GREY         "\033[0;90m"

/** @brief Declaração adiantada da estrutura ASTNode para uso nas funções de impressão. */
typedef struct ASTNode ASTNode;

/** @brief Impressão estilizada para o título, subtítulo e fases do projeto de compilador.
 *  @details O estilo utilizado para o título segue uma tipografia Heavy Block estilizado com bold cyan.
 *  @return void
 */
void printTitle();

/** @brief Impressão da tabela de símbolos.
 *  @param hash_table Tabela de símbolos a ser impressa.
 *  @return void
 */
void printSymbolTable(SymbolTable **hash_table);

/** @brief Impressão da tabela de erros.
 *  @param error_table Tabela de erros a ser impressa.
 *  @return void
 */
void printErrorTable(ErrorTable **error_table);

/** @brief Impressão de mensagens estilizadas conforme o tipo.
 *  @param message Mensagem a ser impressa.
 *  @param messageType Tipo da mensagem (1: Info, 2: Warning, 3: Error).
 *  @return void
 */
void printMessage(char *message, int messageType);

/** @brief Impressão do divisor de conteúdo entre as fases do compilador impressas na função principal.
 *  @return void
 */
void printDivisor();

/** @brief Recebe uma entrada do usuário com uma mensagem estilizada.
 *  @param message Mensagem a ser exibida para o usuário.
 *  @param entryType Tipo de entrada (1: Input).
 *  @return void
 */
void receiveEntry(char *message, int entryType);

/** @brief Impressão de subtítulos estilizados.
 *  @param message Mensagem do subtítulo a ser impressa.
 *  @return void
 */
void printSubtitle(char *message);

// ================================================================================================================================================
// AUTÔMATO DE RECONHECIMENTO DE COMENTÁRIOS DE BLOCO
// ================================================================================================================================================

/** @brief Módulo responsável pela validação de comentários de bloco em MiniPascal.
 *  @details Este autômato possui quatro estados: C_Q0 (estado inicial), C_Q1 (após ler '/'), C_Q2 (dentro do comentário), C_Q3 (após ler '*'), e C_Q4 (estado de aceitação após ler '*'e '/')
 *  @details O autômato aceita strings que começam com / e * e terminam com * e /, permitindo qualquer caractere entre eles, incluindo quebras de linha.
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (104108
 */

/** @brief Estados do autômato para validação de comentários de bloco.
 *  @property C_Q0 Estado inicial, aguardando o início do comentário.
 *  @property C_Q1 Estado após a leitura de '/'.
 *  @property C_Q2 Estado dentro do comentário após a leitura de '/*'.
 *  @property C_Q3 Estado após a leitura de '*' dentro do comentário.
 *  @property C_Q4 Estado final, indicando o término do comentário.
 */
typedef enum {
    C_Q0, C_Q1, C_Q2, C_Q3, C_Q4
} CommentState;

/** @brief Validação de comentários de bloco em MiniPascal.
 *  @param input String de entrada a ser verificada.
 *  @return 0 se a entrada for um comentário válido e 1 caso contrário.
 */
int comment_afn(const char *input);

// ================================================================================================================================================
// AUTÔMATO DE RECONHECIMENTO DE TIPOS DE DADOS
// ================================================================================================================================================

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

// ================================================================================================================================================
// AUTÔMATO DE RECONHECIMENTO DE DELIMITADORES
// ================================================================================================================================================

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


// ================================================================================================================================================
// MANIPULAÇÃO DE ARQUIVOS
// ================================================================================================================================================

/** @brief Recebe o arquivo com base em um caminho fornecido como argumento.
 *  @param file Ponteiro para o arquivo a ser aberto.
 *  @param filepath Caminho do arquivo a ser aberto.
 *  @param mode Modo de abertura do arquivo (r, w, a, rb, wb, ab).
 *  @return Retorna 0 se o arquivo foi aberto com sucesso, ou um código de erro caso contrário.
 */
int openFile(FILE **file, const char *filepath, char *mode);

/** @brief Valida o caminho do arquivo fornecido pelo usuário.
 *  @param filepath Caminho do arquivo a ser validado
 *  @return Retorna 0 se o caminho for válido, ou 1 caso contrário.
 */
int validateFilePath(const char *filepath);

/** @brief Escreve o conteúdo fornecido em um arquivo no caminho especificado.
 *  @param filepath Caminho do arquivo onde o conteúdo será escrito.
 *  @param content Conteúdo a ser escrito no arquivo.
 *  @return Retorna 0 se a escrita foi bem-sucedida, ou um código de erro caso contrário.
 */
int writeOnFile(const char *filepath, const char *content);

/** @brief Lê o conteúdo de um arquivo no caminho especificado.
 *  @param filepath Caminho do arquivo a ser lido.
 *  @param contentBuffer Ponteiro para o buffer onde o conteúdo lido será armazenado.
 *  @return Retorna o número de bytes lidos, ou um código de erro caso contrário.
 */
int readFileContent(const char *filepath, char **contentBuffer);

/** @brief Cria um novo arquivo no caminho especificado.
 *  @param filepath Caminho do arquivo a ser lido.
 *  @return Retorna 0 se o arquivo foi criado com sucesso, ou um código de erro caso contrário.
 */
int createFile(const char *filepath);

/** @brief Fecha o arquivo fornecido.
 *  @param file Ponteiro para o arquivo a ser fechado.
 *  @return Retorna 0 se o arquivo foi fechado com sucesso, ou um código de erro caso contrário.
 */
int closeFile(FILE *file);

// ================================================================================================================================================
// AUTÔMATO DE RECONHECIMENTO DE IDENTIFICADORES
// ================================================================================================================================================

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

// ================================================================================================================================================
// GERAÇÃO DE CÓDIGO INTERMEDIÁRIO
// ================================================================================================================================================

/** @brief Enumerador para representar os tipos de operações TAC.
 *  @property TAC_ASSIGN Atribuição simples, como x = y
 *  @property TAC_ASSIGN_LITERAL Atribuição de literal, como x = 5
 *  @property TAC_ADD Adição, como x = y + z
 *  @property TAC_SUB Subtração, como x = y - z
 *  @property TAC_MUL Multiplicação, como x = y * z
 *  @property TAC_DIV Divisão, como x = y / z
 *  @property TAC_MOD Módulo, como x = y mod z
 *  @property TAC_LT Menor que, como x = y < z
 *  @property TAC_GT Maior que, como x = y > z
 *  @property TAC_LE Menor ou igual, como x = y <= z
 *  @property TAC_GE Maior ou igual, como x = y >= z
 *  @property TAC_EQ Igualdade, como x = y == z
 *  @property TAC_NE Desigualdade, como x = y != z
 *  @property TAC_AND Operação lógica E, como x = y && z
 *  @property TAC_OR Operação lógica OU, como x = y || z
 *  @property TAC_NOT Operação lógica NÃO, como x = !y
 *  @property TAC_LABEL Definição de rótulo, como label:
 *  @property TAC_JUMP Salto incondicional, como goto label
 *  @property TAC_JUMP_IF_FALSE Salto condicional se falso, como if !x goto label
 *  @property TAC_JUMP_IF_TRUE Salto condicional se verdadeiro, como if x goto label
 *  @property TAC_CALL Chamada de função, como call function(arg1, arg2, ...)
 *  @property TAC_RETURN Retorno de função, como return x
 *  @property TAC_PARAM Passagem de parâmetro, como param x
 *  @property TAC_READ Leitura de entrada, como read x
 *  @property TAC_WRITE Escrita de saída, como write x
 *  @property TAC_ARRAY_ACCESS Acesso a array, como x = array[y]
 *  @property TAC_ARRAY_ASSIGN Atribuição a array, como array[x] = y
 *  @property TAC_FUNCTION_START Início de função, como function name:
 *  @property TAC_FUNCTION_END Fim de função, como end function
 */
typedef enum {
    TAC_ASSIGN,           
    TAC_ASSIGN_LITERAL,   
    TAC_ADD,              
    TAC_SUB,              
    TAC_MUL,              
    TAC_DIV,              
    TAC_MOD,              
    TAC_LT,               
    TAC_GT,               
    TAC_LE,               
    TAC_GE,               
    TAC_EQ,               
    TAC_NE,               
    TAC_AND,              
    TAC_OR,               
    TAC_NOT,              
    TAC_LABEL,            
    TAC_JUMP,             
    TAC_JUMP_IF_FALSE,    
    TAC_JUMP_IF_TRUE,     
    TAC_CALL,             
    TAC_RETURN,           
    TAC_PARAM,            
    TAC_READ,             
    TAC_WRITE,            
    TAC_ARRAY_ACCESS,     
    TAC_ARRAY_ASSIGN,     
    TAC_FUNCTION_START,   
    TAC_FUNCTION_END 
} TACOpType;

/** @brief Estrutura para representar uma instrução TAC.
 *  @property op Tipo de operação TAC
 *  @property result Destino (variável resultado)
 *  @property arg1 Primeiro operando
 *  @property arg2 Segundo operando
 *  @property line Número da linha fonte
 */
typedef struct {
    TACOpType op;         
    char *result;         
    char *arg1;           
    char *arg2;           
    int line;             
} TACInstruction;

/** @brief Estrutura para representar o código intermediário (TAC).
 *  @property instructions Array de instruções TAC
 *  @property instruction_count Número de instruções atualmente armazenadas
 *  @property instruction_capacity Capacidade atual do array de instruções
 *  @property temp_counter Contador para variáveis temporárias
 *  @property label_counter Contador para rótulos
 */
typedef struct {
    TACInstruction *instructions;
    int instruction_count;
    int instruction_capacity;
    int temp_counter;     
    int label_counter;    
} IntermediateCode;

/** @brief Cria uma nova estrutura de código intermediário.
 *  @return Ponteiro para a estrutura IntermediateCode criada.
 */
IntermediateCode* createIntermediateCode();

/** @brief Adiciona uma instrução TAC ao código intermediário.
 *  @param code Ponteiro para a estrutura IntermediateCode.
 *  @param op Tipo de operação TAC.
 *  @param result Destino (variável resultado).
 *  @param arg1 Primeiro operando.
 *  @param arg2 Segundo operando.
 *  @param line Número da linha fonte.
 *  @return void
 */
void addTACInstruction(IntermediateCode *code, TACOpType op, const char *result, const char *arg1, const char *arg2, int line);

/** @brief Gera o código intermediário (TAC) a partir da AST.
 *  @param ast Ponteiro para a raiz da Árvore Sintática Abstrata (AST).
 *  @param symtab Ponteiro para a tabela de símbolos.
 *  @param errtab Ponteiro para a tabela de erros.
 *  @return Ponteiro para a estrutura IntermediateCode gerada.
 */
IntermediateCode* generateIntermediateCode(ASTNode *ast, SymbolTable **symtab, ErrorTable *errtab);

/** @brief Imprime o código intermediário no console de forma formatada.
 *  @param code Ponteiro para a estrutura IntermediateCode.
 *  @return void
 */
void printIntermediateCode(IntermediateCode *code);

/** @brief Escreve o código intermediário em um arquivo.
 *  @param code Ponteiro para a estrutura IntermediateCode.
 *  @param filepath Caminho do arquivo onde o código será escrito.
 *  @return 0 em caso de sucesso, -1 em caso de erro.
 */
int writeIntermediateCodeToFile(IntermediateCode *code, const char *filepath);

/** @brief Libera a memória alocada para o código intermediário.
 *  @param code Ponteiro para a estrutura IntermediateCode a ser liberada.
 *  @return void
 */
void freeIntermediateCode(IntermediateCode *code);

/** @brief Converte o tipo de operação TAC para sua representação em string.
 *  @param op Tipo de operação TAC.
 *  @return String representando o tipo de operação.
 */
const char* tacOpToString(TACOpType op);

/**
 * @brief Print intermediate code in sequential format (non-tabular)
 * @param code Intermediate code to print
 */
void printIntermediateCodeSequential(IntermediateCode *code);

/**
 * @brief Write intermediate code to file in sequential format
 * @param code Intermediate code
 * @param filepath Path to output file
 * @return 0 on success, -1 on failure
 */
int writeIntermediateCodeSequential(IntermediateCode *code, const char *filepath);

// ================================================================================================================================================
// AUTÔMATO DE RECONHECIMENTO DE PALAVRAS-CHAVE
// ================================================================================================================================================

/** @brief Estrutura para representar o estado do autômato de palavras-chave.
 *  @property state Estado atual do autômato.
 *  @property is_final Indica se o estado atual é um estado final.
 */
typedef struct {
    int state;
    int is_final;
} KeywordState;

/** @brief Verifica se um caractere é válido para palavras-chave.
 *  @param c Caractere a ser verificado.
 *  @return 1 se o caractere for válido, 0 caso contrário.
 */
int is_keyword_char(char c);

/** @brief Verifica se a string de entrada é uma palavra-chave válida.
 *  @param input String de entrada a ser verificada.
 *  @return 1 se a entrada for uma palavra-chave válida, 0 caso contrário.
 */
int is_keyword(const char* input);

/** @brief Função de transição do autômato de palavras-chave.
 *  @param current Estado atual do autômato.
 *  @param c Caractere lido da entrada.
 *  @return Próximo estado do autômato.
 */
KeywordState transition(KeywordState current, char c);

// ================================================================================================================================================
// ANÁLISE LÉXICA
// ================================================================================================================================================

#define TOKEN_IDENTIFIER    10
#define TOKEN_KEYWORD       11
#define TOKEN_LITERAL       12
#define TOKEN_DATATYPE      13
#define TOKEN_DELIMITER     14
#define TOKEN_COMMENT       15
#define TOKEN_EOF           0
#define TOKEN_ERROR         0

#define TOKEN_VALUE(t) ((t).lexeme)

/** @brief Estrutura para representar um token produzido pela análise léxica.
 *  @property type Código inteiro que descreve a categoria do token (identificador, palavra-chave, operador, delimitador, literal, etc.). Constantes de token em todo o projeto (se houver) determinam o mapeamento numérico.
 *  @property lexeme String terminada em nulo que contém o lexema do token. Buffer de tamanho fixo alocado dentro da estrutura Token (64 bytes).
 *  @property line Linha onde o token foi encontrado no código fonte.
 */
typedef struct Token {
    int type;
    char lexeme[64];
    int line;
} Token;

/** @brief Realiza a análise léxica com base em uma entrada de texto.
 *  @param input String de entrada terminada em nulo a ser tokenizada.
 *  @param filename Nome do arquivo sendo analisado.
 *  @return Ponteiro para o primeiro elemento de um array dinamicamente alocado de Tokens. O chamador deve usar free() no ponteiro retornado quando terminar.
 */
Token* lexicalAnalysis(const char *input, const char *filename, SymbolTable **symtab, ErrorTable *errtab);

// ================================================================================================================================================
// AUTÔMATO DE RECONHECIMENTO DE OPERADORES
// ================================================================================================================================================

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

// ================================================================================================================================================
// ANÁLISE SEMÂNTICA
// ================================================================================================================================================

/** @brief Função para realizar a análise semântica do código MiniPascal.
 *  @param root Ponteiro para o nó raiz da Árvore Sintática Abstrata (AST).
 *  @param symtab Ponteiro para a tabela de símbolos.
 *  @param errtab Ponteiro para a tabela de erros.
 *  @param filename Nome do arquivo fonte sendo analisado.
 *  @return 0 se a análise semântica for bem-sucedida, 1 caso contrário.
 */
int semanticAnalysis(ASTNode *root, SymbolTable **symtab, ErrorTable *errtab, const char *filename);

// ================================================================================================================================================
// ANÁLISE SINTÁTICA
// ================================================================================================================================================

/** @brief Estrutura para representar um nó na Árvore Sintática Abstrata (AST).
 *  @property type Tipo do nó (expressão, declaração, etc.)
 *  @property value Valor associado ao nó (se aplicável)
 *  @property line Linha onde o nó foi encontrado no código fonte
 *  @property children Array de ponteiros para os nós filhos
 *  @property child_count Número de filhos atualmente armazenados
 *  @property child_capacity Capacidade atual do array de filhos
 */
typedef struct ASTNode {
    char *type;              
    char *value;             
    int line;                
    struct ASTNode **children;
    int child_count;
    int child_capacity;
} ASTNode;

/** @brief Estrutura para representar o estado do parser.
 *  @property tokens Array de tokens a serem analisados
 *  @property current Índice do token atual sendo analisado
 *  @property symtab Ponteiro para a tabela de símbolos
 *  @property errtab Ponteiro para a tabela de erros
 *  @property error_count Contador de erros encontrados durante a análise
 *  @property root Ponteiro para o nó raiz da Árvore Sintática Abstrata (AST)
 */
typedef struct {
    Token *tokens;           
    int current;             
    SymbolTable **symtab;    
    ErrorTable *errtab;      
    int error_count;
    ASTNode *root;           
} Parser;

/** @brief Cria um novo nó na Árvore Sintática Abstrata (AST).
 *  @param type Tipo do nó (expressão, declaração, etc.)
 *  @param value Valor associado ao nó (se aplicável)
 *  @param line Linha onde o nó foi encontrado no código fonte
 *  @return Ponteiro para o nó AST criado.
 */
ASTNode* createASTNode(const char *type, const char *value, int line);

/** @brief Adiciona um nó filho a um nó pai na Árvore Sintática Abstrata (AST).
 *  @param parent Ponteiro para o nó pai.
 *  @param child Ponteiro para o nó filho a ser adicionado.
 *  @return void
 */
void astAddChild(ASTNode *parent, ASTNode *child);

/** @brief Libera a memória alocada para a Árvore Sintática Abstrata (AST).
 *  @param node Ponteiro para o nó raiz da AST a ser liberada.
 *  @return void
 */
void freeAST(ASTNode *node);

/** @brief Imprime a Árvore Sintática Abstrata (AST) de forma formatada no console.
 *  @param node Ponteiro para o nó raiz da AST a ser impressa.
 *  @return void
 */
void printAST(ASTNode *node);

/** @brief Imprime a Árvore Sintática Abstrata (AST) em um arquivo.
 *  @param node Ponteiro para o nó raiz da AST a ser impressa.
 *  @param filepath Caminho do arquivo onde a AST será escrita.
 *  @return void
 */
void printASTToFile(ASTNode *node, const char *filepath);

/** @brief Cria e inicializa o parser com os tokens fornecidos.
 *  @param tokens Array de tokens a serem analisados.
 *  @param symtab Ponteiro para a tabela de símbolos.
 *  @param errtab Ponteiro para a tabela de erros.
 *  @return Parser Estrutura do parser inicializada.
 */
Parser createParser(Token *tokens, SymbolTable **symtab, ErrorTable *errtab);

/** @brief Realiza a análise sintática dos tokens fornecidos.
 *  @param parser Ponteiro para a estrutura do parser.
 *  @return 0 se a análise sintática for bem-sucedida, 1 caso contrário.
 */
int syntacticAnalysis(Parser *parser);

// ================================================================================================================================================
// IMPLEMENTAÇÃO DA FUNÇÃO MAIN E DEMAIS FUNÇÕES UTILIZADAS NAS FASES LÉXICA, SINTÁTICA, SEMÂNTICA E GERAÇÃO DE CÓDIGO INTERMEDIÁRIO
// ================================================================================================================================================

int main(int argc, char *argv[]) {
    SymbolTable **symbolTable = NULL;
    ErrorTable *errorTable = NULL;
    
    FILE *file = NULL;

    char *contentBuffer = NULL;
    char filepath[256] = {0};

    printTitle();
    printDivisor();

    if (argc == 1) {
        printMessage("nenhum arquivo foi fornecido como entrada.", 2);
        printMessage("realizando a leitura do arquivo manualmente...", 2);

        receiveEntry("insira o caminho do arquivo a ser lido: ", 1);
        scanf("%255s", filepath);

        while(validateFilePath(filepath)) {
            printMessage("não foi possível identificar o arquivo no caminho fornecido. Tente novamente...", 3);
            receiveEntry("insira o caminho do arquivo a ser lido: ", 1);
            scanf("%255s", filepath);
        }

        if (openFile(&file, filepath, "r")) {
            printMessage("falha ao abrir o arquivo.", 3);
            return EXIT_FAILURE;

        } else {
            printMessage("arquivo aberto com sucesso!", 1);
            readFileContent(filepath, &contentBuffer);
            
            if (!contentBuffer || strlen(contentBuffer) == 0) {
                printMessage("arquivo vazio ou erro ao ler conteúdo.", 3);
                closeFile(file);
                return EXIT_FAILURE;
            }
            
            closeFile(file);
        }

    } else {
        strncpy(filepath, argv[1], sizeof(filepath)-1);
        filepath[sizeof(filepath)-1] = '\0';
        
        if (openFile(&file, filepath, "r")) {
            printMessage("falha ao abrir o arquivo.", 3);
            return EXIT_FAILURE;
        
        } else {
            printMessage("arquivo aberto com sucesso!", 1);
            readFileContent(filepath, &contentBuffer);
        
            if (!contentBuffer || strlen(contentBuffer) == 0) {
                printMessage("arquivo vazio ou erro ao ler conteúdo.", 3);
                closeFile(file);
                return EXIT_FAILURE;
            }
        
            closeFile(file);
        }
    }

    printDivisor();
    printSubtitle("Análise Léxica");

    symbolTable = initSymbolTable();
    
    if (!symbolTable) {
        printMessage("falha ao inicializar tabela de símbolos.", 3);
    
        free(contentBuffer);
        return EXIT_FAILURE;
    }

    errorTable = createErrorTable(256);
    
    if (!errorTable) {
        printMessage("falha ao criar tabela de erros.", 3);
    
        free(contentBuffer);
        return EXIT_FAILURE;
    }

    Token *tokens = lexicalAnalysis(contentBuffer, filepath, symbolTable, errorTable);
    
    if (!tokens) {
        printMessage("falha na análise léxica.", 3);
    
        free(contentBuffer);
        return EXIT_FAILURE;
    }

    createFile("out/tokens.txt");
    writeOnFile("out/tokens.txt", getSymbolTable(symbolTable));
    printSymbolTable(symbolTable);
    
    if (errorTable && errorTable->count > 0) {
        printf("\n");
        printMessage("erros encontrados na análise léxica:", 3);
        printErrorTable(&errorTable);
    
        free(contentBuffer);
        if (tokens) free(tokens);
        if (symbolTable) destroySymbolTable(symbolTable);
        if (errorTable) destroyErrorTable(errorTable);
        return EXIT_FAILURE;
    }

    free(contentBuffer);

    printDivisor();
    printSubtitle("Análise Sintática");

    Parser parser = createParser(tokens, symbolTable, errorTable);
    int syntaxResult = syntacticAnalysis(&parser);

    if (syntaxResult == 0 && !(errorTable && errorTable->count > 0)) {
        printMessage("análise sintática concluída com sucesso!", 1);

    } else {
        printMessage("falha na análise sintática.", 3);
        if (errorTable && errorTable->count > 0) {
            printErrorTable(&errorTable);
        }
        if (parser.root) freeAST(parser.root);
        if (tokens) free(tokens);
        if (errorTable) destroyErrorTable(errorTable);
        if (symbolTable) destroySymbolTable(symbolTable);
        return EXIT_FAILURE;
    }

    if (!parser.root) {
        printMessage("falha ao criar árvore sintática.", 3);
        free(tokens);
        return EXIT_FAILURE;
    }

    createFile("out/ast.txt");
    printASTToFile(parser.root, "out/ast.txt");

    printDivisor();
    printSubtitle("Análise Semântica");

    int semanticResult = semanticAnalysis(parser.root, symbolTable, errorTable, filepath);

    if (semanticResult == 0 && !(errorTable && errorTable->count > 0)) {
        printMessage("análise semântica concluída com sucesso!", 1);

    } else {
        printMessage("falha na análise semântica.", 3);
        if (errorTable && errorTable->count > 0) {
            printErrorTable(&errorTable);
        }
        if (parser.root) freeAST(parser.root);
        if (tokens) free(tokens);
        if (errorTable) destroyErrorTable(errorTable);
        if (symbolTable) destroySymbolTable(symbolTable);
        return EXIT_FAILURE;
    }

    printDivisor();
    printSubtitle("Geração de Código Intermediário");

    IntermediateCode *intermediateCode = generateIntermediateCode(parser.root, symbolTable, errorTable);
    
    if (!intermediateCode) {
        printMessage("falha ao gerar código intermediário.", 3);
        if (parser.root) freeAST(parser.root);
        if (tokens) free(tokens);
        if (errorTable) destroyErrorTable(errorTable);
        if (symbolTable) destroySymbolTable(symbolTable);
        return EXIT_FAILURE;
    }

    printMessage("código intermediário gerado com sucesso!", 1);
    
    printf("\n");
    printMessage("Formato Sequencial (Assembly-like):", 2);
    printIntermediateCodeSequential(intermediateCode);
    
    createFile("out/intermediate_code_sequential.txt");
    writeIntermediateCodeSequential(intermediateCode, "out/intermediate_code_sequential.txt");
    
    printf("\n");
    printMessage("Formato Tabular (para referência):", 2);
    printIntermediateCode(intermediateCode);
    
    createFile("out/intermediate_code_table.txt");
    writeIntermediateCodeToFile(intermediateCode, "out/intermediate_code_table.txt");

    printDivisor();

    if (intermediateCode) {
        freeIntermediateCode(intermediateCode);
    }
    if (parser.root) {
        freeAST(parser.root);
    }
    if (tokens) {
        free(tokens);
    }
    if (errorTable) {
        destroyErrorTable(errorTable);
    }
    if (symbolTable) {
        destroySymbolTable(symbolTable);
    }

    return 0;
}

int comment_afn(const char *input) {
    CommentState state = C_Q0;

    for (int i = 0; input[i] != '\0'; i++) {
        char c = input[i];

        switch (state) {
            case C_Q0:
                if (c == '/') state = C_Q1;
                else state = C_Q0;
                break;

            case C_Q1:
                if (c == '*') state = C_Q2;
                else if (c == '/') state = C_Q1;
                else state = C_Q0;
                break;

            case C_Q2:
                if (c == '*') state = C_Q3;
                else state = C_Q2; 
                break;

            case C_Q3:
                if (c == '/') state = C_Q4; 
                else if (c == '*') state = C_Q3; 
                else state = C_Q2; 
                break;

            case C_Q4:
                break;
        }

        if (state == C_Q4 && input[i+1] != '\0') {
            return 1;
        }
    }

    return state == C_Q4 ? 0 : 1;
}

DatatypeState datatype_next_state(DatatypeState current_state, char caracter) {
    switch (current_state) {
        case D_Q0:
            if (caracter == 'i') return D_Q1;
            if (caracter == 'b') return D_Q8;
            return D_Q14;
        
        case D_Q1:
            if (caracter == 'n') return D_Q2;
            return D_Q15; 
        case D_Q2:
            if (caracter == 't') return D_Q3;
            return D_Q15; 
        case D_Q3:
            if (caracter == 'e') return D_Q4;
            return D_Q15; 
        case D_Q4:
            if (caracter == 'g') return D_Q5;
            return D_Q15; 
        case D_Q5:
            if (caracter == 'e') return D_Q6;
            return D_Q15; 
        case D_Q6:
            if (caracter == 'r') return D_Q7; 
            return D_Q15; 
        
        case D_Q8:
            if (caracter == 'o') return D_Q9;
            return D_Q14; 
        case D_Q9:
            if (caracter == 'o') return D_Q10;
            return D_Q14; 
        case D_Q10:
            if (caracter == 'l') return D_Q11;
            return D_Q14; 
        case D_Q11:
            if (caracter == 'e') return D_Q12;
            return D_Q14; 
        case D_Q12:
            if (caracter == 'a') return D_Q13;
            return D_Q14; 
        case D_Q13:
            if (caracter == 'n') return D_Q7; 
            return D_Q14; 

        case D_Q7:
        case D_Q14: 
        case D_Q15: 
            return current_state; 
        
        default:
            return D_Q14; 
    }
}

int datatype_afn(const char *start) {
    DatatypeState current_state = D_Q0;
    int length = strlen(start);
    int i = 0;

    while (i < length) {
    current_state = datatype_next_state(current_state, start[i]);
        if (current_state == D_Q14 || current_state == D_Q15) {
            return 0;
        }
        
        i++;
    }

    if (current_state == D_Q7) {
        return 1;
    }

    return 0;
}

DelimiterState delimiter_next_state(DelimiterState current_state, char character) {
    switch (current_state) {
        case DEL_Q1:
            if (isalpha(character)) return DEL_Q2;
            if (character == 'e') return DEL_Q7;
            return DEL_Q6;

        case DEL_Q2:
            if (isalnum(character)) return DEL_Q2;
            if (character == ',') return DEL_Q3;
            if (character == ':') return DEL_Q4;
            return DEL_Q6;

        case DEL_Q3:
            if (isalpha(character)) return DEL_Q2;
            return DEL_Q6;

        case DEL_Q4:
            if (isalpha(character)) return DEL_Q5;
            return DEL_Q6;

        case DEL_Q5:
            if (isalnum(character)) return DEL_Q5;
            if (character == ';') return DEL_Q8;
            return DEL_Q6;

        case DEL_Q7:
            if (character == 'n') return DEL_Q9;
            return DEL_Q6;

        case DEL_Q9:
            if (character == 'd') return DEL_Q10;
            return DEL_Q6;

        case DEL_Q10:
            if (character == '.') return DEL_Q11;
            return DEL_Q6;

        case DEL_Q8:
        case DEL_Q11:
            return current_state;

        case DEL_Q6:
            return DEL_Q6;

        default:
            return DEL_Q6;
    }
}

int check_datatype(const char *input, int start, int end) {
    char type[32];
    int j = 0;
    
    for (int i = start; i < end && j < 31; i++) {
        type[j++] = input[i];
    }
    type[j] = '\0';
    
    return datatype_afn(type);
}

int delimiter_afn(const char *input) {
    DelimiterState current_state = DEL_Q1;
    int length = strlen(input);
    int i = 0;
    int type_start = -1;

    while (i < length) {
        if (current_state == DEL_Q5 && type_start == -1) {
            type_start = i;
        }
        
    current_state = delimiter_next_state(current_state, input[i]);
        
        if (current_state == DEL_Q6) {
            return 0;
        }

        if (current_state == DEL_Q8 && type_start != -1) {
            if (!check_datatype(input, type_start, i)) {
                return 0;
            }
        }
        
        i++;
    }

    return (current_state == DEL_Q8 || current_state == DEL_Q11);
}


int identifier_afn(const char *input) {
    IdentifierState state = ID_Q0;
    int i = 0;

    while (input[i] != '\0') {
        char c = input[i];

        switch (state) {
            case ID_Q0:
                if (isalpha(c) || c == '_') {
                    state = ID_Q1;
                } else if (isdigit(c)) {
                    state = ID_Q2; 
                } else {
                    state = ID_Q2;
                }
                break;

            case ID_Q1:
                if (isalnum(c) || c == '_') {
                    state = ID_Q1;
                } else {
                    state = ID_Q2;
                }
                break;

            case ID_Q2:
                return false; 
        }

        i++;
    }
    
    return (state == ID_Q1) ? 1 : 0;
}


int is_keyword_char(char c) {
    return (c >= 'a' && c <= 'z');
}

KeywordState transition(KeywordState current, char c) {
    KeywordState next = {0, 0};
    
    switch (current.state) {
        case 0:
            if (c == 'p') next.state = 1;
            else if (c == 'v') next.state = 8;
            else if (c == 'b') next.state = 13;
            else if (c == 'e') next.state = 17;
            else if (c == 't') next.state = 24;
            else if (c == 'i') next.state = 29;
            else if (c == 'w') next.state = 35;
            else if (c == 'd') next.state = 41;
            else if (c == 'f') next.state = 43;
            else if (c == 'c') next.state = 52;
            else if (c == 'r') next.state = 58;
            else if (c == 'a') next.state = 66;
            else if (c == 'o') next.state = 72;
            else if (c == 'u') next.state = 75;
            break;
        case 1:
            if (c == 'r') next.state = 2;
            break;
        case 2:
            if (c == 'o') next.state = 3;
            break;
        case 3:
            if (c == 'g') next.state = 4;
            break;
        case 4:
            if (c == 'r') next.state = 5;
            break;
        case 5:
            if (c == 'a') next.state = 6;
            break;
        case 6:
            if (c == 'm') {
                next.state = 7;
                next.is_final = 1;
            }
            break;
        case 8:
            if (c == 'a') next.state = 9;
            break;
        case 9:
            if (c == 'r') {
                next.state = 10;
                next.is_final = 1;
            }
            break;
        case 13:
            if (c == 'e') next.state = 14;
            break;
        case 14:
            if (c == 'g') next.state = 15;
            break;
        case 15:
            if (c == 'i') next.state = 16;
            break;
        case 16:
            if (c == 'n') {
                next.state = 17;
                next.is_final = 1;
            }
            break;
        case 17:
            if (c == 'n') next.state = 18;
            else if (c == 'l') next.state = 19;
            break;
        case 19:
            if (c == 's') next.state = 20;
            break;
        case 20:
            if (c == 'e') {
                next.state = 21;
                next.is_final = 1;
            }
            break;
        case 24:
            if (c == 'h') next.state = 25;
            else if (c == 'o') next.state = 26;
            else if (c == 'y') {
                next.state = 27;
                next.is_final = 1;
            }
            break;
        case 25:
            if (c == 'e') next.state = 26;
            break;
        case 26:
            if (c == 'n') {
                next.state = 27;
                next.is_final = 1;
            }
            break;
        case 29:
            if (c == 'f') {
                next.state = 30;
                next.is_final = 1;
            }
            break;
        case 35:
            if (c == 'h') next.state = 36;
            break;
        case 36:
            if (c == 'i') next.state = 37;
            break;
        case 37:
            if (c == 'l') next.state = 38;
            break;
        case 38:
            if (c == 'e') {
                next.state = 39;
                next.is_final = 1;
            }
            break;
        case 41:
            if (c == 'o') {
                next.state = 42;
                next.is_final = 1;
            }
            break;
        case 43:
            if (c == 'o') next.state = 44;
            else if (c == 'u') next.state = 45;
            break;
        case 44:
            if (c == 'r') {
                next.state = 45;
                next.is_final = 1;
            }
            break;
        case 52:
            if (c == 'o') next.state = 53;
            break;
        case 53:
            if (c == 'n') next.state = 54;
            break;
        case 54:
            if (c == 's') next.state = 55;
            break;
        case 55:
            if (c == 't') {
                next.state = 56;
                next.is_final = 1;
            }
            break;
        case 58:
            if (c == 'e') next.state = 59;
            break;
        case 59:
            if (c == 'p') next.state = 60;
            else if (c == 'a') next.state = 61;
            else if (c == 'c') next.state = 62;
            break;
        case 60:
            if (c == 'e') next.state = 61;
            break;
        case 61:
            if (c == 'a') next.state = 62;
            break;
        case 62:
            if (c == 't') {
                next.state = 63;
                next.is_final = 1;
            }
            break;
        case 66:
            if (c == 'r') next.state = 67;
            break;
        case 67:
            if (c == 'r') next.state = 68;
            break;
        case 68:
            if (c == 'a') next.state = 69;
            break;
        case 69:
            if (c == 'y') {
                next.state = 70;
                next.is_final = 1;
            }
            break;
        case 72:
            if (c == 'f') {
                next.state = 73;
                next.is_final = 1;
            }
            break;
        case 75:
            if (c == 'n') next.state = 76;
            break;
        case 76:
            if (c == 't') next.state = 77;
            break;
        case 77:
            if (c == 'i') next.state = 78;
            break;
        case 78:
            if (c == 'l') {
                next.state = 79;
                next.is_final = 1;
            }
            break;
    }
    return next;
}

int is_keyword(const char* input) {
    KeywordState current = {0, 0};
    int i = 0;

    while (input[i] != '\0') {
        char c = input[i];
        current = transition(current, c);
        if (current.state == 0) {
            return 0;
        }
        i++;
    }

    return current.is_final;
}


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
            if (c == '>') return OP_Q2;
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


void printTitle() {
    printf(BOLD_CYAN
        "███╗   ███╗██╗███╗   ██╗██╗██████╗  █████╗ ███████╗ ██████╗ █████╗ ██╗          ██████╗ ██████╗ ███╗   ███╗██████╗ ██╗██╗     ███████╗██████╗     ██████╗ ██████╗  ██████╗      ██╗███████╗ ██████╗████████╗\n"
        "████╗ ████║██║████╗  ██║██║██╔══██╗██╔══██╗██╔════╝██╔════╝██╔══██╗██║         ██╔════╝██╔═══██╗████╗ ████║██╔══██╗██║██║     ██╔════╝██╔══██╗    ██╔══██╗██╔══██╗██╔═══██╗     ██║██╔════╝██╔════╝╚══██╔══╝\n"
        "██╔████╔██║██║██╔██╗ ██║██║██████╔╝███████║███████╗██║     ███████║██║         ██║     ██║   ██║██╔████╔██║██████╔╝██║██║     █████╗  ██████╔╝    ██████╔╝██████╔╝██║   ██║     ██║█████╗  ██║        ██║   \n"
        "██║╚██╔╝██║██║██║╚██╗██║██║██╔═══╝ ██╔══██║╚════██║██║     ██╔══██║██║         ██║     ██║   ██║██║╚██╔╝██║██╔═══╝ ██║██║     ██╔══╝  ██╔══██╗    ██╔═══╝ ██╔══██╗██║   ██║██   ██║██╔══╝  ██║        ██║   \n"
        "██║ ╚═╝ ██║██║██║ ╚████║██║██║     ██║  ██║███████║╚██████╗██║  ██║███████╗    ╚██████╗╚██████╔╝██║ ╚═╝ ██║██║     ██║███████╗███████╗██║  ██║    ██║     ██║  ██║╚██████╔╝╚█████╔╝███████╗╚██████╗   ██║   \n"
        "╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝╚═╝     ╚═╝  ╚═╝╚══════╝ ╚═════╝╚═╝  ╚═╝╚══════╝     ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝    ╚═╝     ╚═╝  ╚═╝ ╚═════╝  ╚════╝ ╚══════╝ ╚═════╝   ╚═╝ \n"
        RESET
        "\n"
        WHITE"      feito por "BOLD_GREEN"Vitor Pereira"RESET" e "BOLD_MAGENTA"Jessica Bispo"RESET"\n"
        BOLD_YELLOW"                Compiladores 2025/2\n"RESET
        "\n"
        BOLD_BLUE"[+] "CYAN"Análise Léxica\n"
        BOLD_BLUE"[+] "CYAN"Análise Sintática\n"
        BOLD_BLUE"[+] "CYAN"Análise Semântica\n"
        BOLD_BLUE"[+] "CYAN"Geração de Código Intermediário\n"RESET
    );

}

void printErrorTable(ErrorTable **error_table) {
    printf("\n%-6s | %-20s | %-55s | %-30s | %-5s\n", "TYPE", "MESSAGE", "SNIPPET", "FILENAME", "LINE");
    printf("-------------------------------------------------------------------------------------------------------------------------------\n");

    if (!error_table || !(*error_table)) {
        printf("Tabela de erros vazia.\n");
        return;
    }

    ErrorTable *table = *error_table;
    
    if (table->count == 0) {
        printf("Tabela de erros vazia.\n");
        return;
    }

    int found = 0;
    for (unsigned int i = 0; i < table->size; ++i) {
        Error *current = table->errors[i];
        while (current) {
            printf("%-6s | %-20s | %-55s | %-30s | %-5d\n",
                   "ERROR",
                   current->description && current->description[0] ? current->description : "-",
                   current->snippet && current->snippet[0] ? current->snippet : "-",
                   current->filename && current->filename[0] ? current->filename : "-",
                   current->line);
            found = 1;
            current = current->next;
        }
    }

    if (!found) {
        printf("Tabela de erros vazia.\n");
    }
}

void printSymbolTable(SymbolTable **hash_table) {
        printf("\n%-15s | %-15s | %-12s | %-12s | %-10s | %-8s | %-15s | %-18s | %-15s | %-12s | %-5s\n",
            "SYMBOL", "CATEGORY", "TYPE", "SCOPE", "ADDRESS", "SIZE", "PARAMETERS", "LITERAL_VALUE", "DECLARED_IN", "VISIBILITY", "LINE");
        printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    if (!hash_table) {
        printf("Tabela de símbolos vazia.\n");
        return;
    }

    int found = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        SymbolTable *current = hash_table[i];
        while (current) {
            char size_buf[32];
            const char *size_str = "-";
            if (current->size != 0) {
                snprintf(size_buf, sizeof(size_buf), "%d", current->size);
                size_str = size_buf;
            }
            printf("%-15s | %-15s | %-12s | %-12s | %-10s | %-8s | %-15s | %-18s | %-15s | %-12s | %-5d\n",
                   current->name && current->name[0] ? current->name : "-",
                   current->category && current->category[0] ? current->category : "-",
                   current->type && current->type[0] ? current->type : "-",
                   current->scope && current->scope[0] ? current->scope : "-",
                   current->address && current->address[0] ? current->address : "-",
                   size_str,
                   current->parameters && current->parameters[0] ? current->parameters : "-",
                   current->literal_value && current->literal_value[0] ? current->literal_value : "-",
                   current->declared_in && current->declared_in[0] ? current->declared_in : "-",
                   current->visibility && current->visibility[0] ? current->visibility : "-",
                   current->line);
            found = 1;
            current = current->next;
        }
    }

    if (!found) {
        printf("Tabela de símbolos vazia.\n");
    }
}

void printSubtitle(char *message) {
    printf(BOLD_YELLOW"%s\n"RESET, message);
}

void printMessage(char *message, int messageType) {
    switch (messageType) {
        case 1:
            printf(BOLD_GREEN"[INFO]: "RESET"%s\n", message);
            break;
        case 2:
            printf(BOLD_YELLOW"[WARNING]: "RESET"%s\n", message);
            break;
        case 3:
            printf(BOLD_RED"[ERROR]: "RESET"%s\n", message);
            break;
        default:
            printf("[UNKNOWN]: %s\n", message);
            break;
    }
}

void printDivisor() {
    printf(GREY"\n+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n\n"RESET);
}

void receiveEntry(char *message, int entryType) {
    switch (entryType) {
        case 1:
            printf(BOLD_GREEN"[INPUT]: "RESET"%s", message);
            break;
    }
}


ErrorTable* createErrorTable(unsigned int size) {
    ErrorTable* table = (ErrorTable*)malloc(sizeof(ErrorTable));
    if (!table) return NULL;

    table->errors = (Error**)calloc(size, sizeof(Error*));
    if (!table->errors) {
        free(table);
        return NULL;
    }

    table->size = size;
    table->count = 0;
    return table;
}

unsigned int hashError(const char* name, unsigned int tableSize) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash * 31 + *name) % tableSize;
        name++;
    }
    return hash;
}

int insertError(ErrorTable* table, const char* name, const char* description, ErrorType type, int line, const char* filename) {
    if (!table || !name || !description) return -1;

    unsigned int index = hashError(name, table->size);
    Error* newError = (Error*)malloc(sizeof(Error));
    if (!newError) return -1;

    newError->name = strdup(name);
    newError->description = strdup(description);
    newError->timestamp = time(NULL);
    newError->type = type;
    newError->line = line;
    newError->filename = filename ? strdup(filename) : strdup("<unknown>");
    newError->snippet = NULL;
    newError->next = NULL;

    if (!newError->name || !newError->description || !newError->filename) {
        free(newError->name);
        free(newError->description);
        free(newError->filename);
        free(newError->snippet);
        free(newError);
        return -1;
    }

    if (filename && line > 0 && strcmp(filename, "<unknown>") != 0) {
        FILE *f = fopen(filename, "r");
        if (f) {
            char buf[2048];
            int cur = 1;
            while (fgets(buf, sizeof(buf), f)) {
                if (cur == line) {
                    /* Trim trailing newline */
                    size_t l = strlen(buf);
                    while (l > 0 && (buf[l-1] == '\n' || buf[l-1] == '\r')) { buf[--l] = '\0'; }
                    newError->snippet = strdup(buf[0] ? buf : "-");
                    break;
                }
                cur++;
            }
            fclose(f);
        }
    }
    if (!newError->snippet) newError->snippet = strdup("-");

    if (table->errors[index]) {
        newError->next = table->errors[index];
    }
    table->errors[index] = newError;
    table->count++;

    return 0;
}

Error* findError(ErrorTable* table, const char* name) {
    if (!table || !name) return NULL;

    unsigned int index = hashError(name, table->size);
    Error* current = table->errors[index];

    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

int removeError(ErrorTable* table, const char* name) {
    if (!table || !name) return -1;

    unsigned int index = hashError(name, table->size);
    Error* current = table->errors[index];
    Error* prev = NULL;

    while (current) {
        if (strcmp(current->name, name) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                table->errors[index] = current->next;
            }

            free(current->name);
            free(current->description);
            free(current->snippet);
            free(current->filename);
            free(current);
            table->count--;
            return 0;
        }
        prev = current;
        current = current->next;
    }

    return -1;
}

void destroyErrorTable(ErrorTable* table) {
    if (!table) return;

    for (unsigned int i = 0; i < table->size; i++) {
        Error* current = table->errors[i];
        while (current) {
            Error* next = current->next;
            free(current->name);
            free(current->description);
            free(current->snippet);
            free(current->filename);
            free(current);
            current = next;
        }
    }

    free(table->errors);
    free(table);
}


int closeFile(FILE *file) {
    if (file == NULL) {
        return 1;
    }
    if (fclose(file) != 0) {
        return 1;
    }
    return 0;
}

int validateFilePath(const char *filepath) {
    return (filepath == NULL || filepath[0] == '\0') ? 1 : 0;
}

int openFile(FILE **file, const char *filepath, char *mode) {
    if (validateFilePath(filepath)) return 1;
    *file = fopen(filepath, mode);
    return (*file != NULL) ? 0 : 1;
}

int readFileContent(const char *filepath, char **contentBuffer) {
    FILE *file;
    long fileSize;
    size_t bytesRead = 0;
    if (openFile(&file, filepath, "r") != 0) {
        return -1;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        closeFile(file);
        return -1;
    }

    fileSize = ftell(file);
    if (fileSize < 0) {
        closeFile(file);
        return -1;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        closeFile(file);
        return -1;
    }

    *contentBuffer = (char *)malloc((size_t)fileSize + 1);
    if (*contentBuffer == NULL) {
        closeFile(file);
        return -1;
    }

    bytesRead = fread(*contentBuffer, 1, (size_t)fileSize, file);
    (*contentBuffer)[bytesRead] = '\0';

    closeFile(file);
    return (int)bytesRead;
}

int createFile(const char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        return 1;
    }
    closeFile(file);
    return 0;
}

int writeOnFile(const char *filepath, const char *content) {
    FILE *file;
    if (openFile(&file, filepath, "w") != 0) {
        return 1;
    }

    if (fputs(content, file) == EOF) {
        closeFile(file);
        return 1;
    }

    closeFile(file);
    return 0;
}


unsigned int hashFunction(const char *str) {
    unsigned int hash = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        hash = (hash * 31 + str[i]) % HASH_TABLE_SIZE;
    }
    return hash;
}

SymbolTable** initSymbolTable() {
    SymbolTable **hash_table = (SymbolTable**)calloc(HASH_TABLE_SIZE, sizeof(SymbolTable*));
    if (!hash_table) {
        fprintf(stderr, "Memory allocation failed for hash table\n");
        return NULL;
    }
    return hash_table;
}

SymbolTable* createSymbol(const char *name, const char *category, const char *type, const char *scope,
                           const char *address, int size, const char *parameters,
                           const char *literal_value, const char *declared_in,
                           const char *visibility, int line) {
    SymbolTable *new_symbol = (SymbolTable *)malloc(sizeof(SymbolTable));
    if (!new_symbol) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    new_symbol->name = name ? strdup(name) : strdup("-");
    new_symbol->category = category ? strdup(category) : strdup("-");
    new_symbol->type = type ? strdup(type) : strdup("-");
    new_symbol->scope = scope ? strdup(scope) : strdup("-");
    new_symbol->address = address ? strdup(address) : strdup("-");
    new_symbol->size = size;
    new_symbol->parameters = parameters ? strdup(parameters) : strdup("-");
    new_symbol->literal_value = literal_value ? strdup(literal_value) : strdup("-");
    new_symbol->declared_in = declared_in ? strdup(declared_in) : strdup("-");
    new_symbol->visibility = visibility ? strdup(visibility) : strdup("-");
    new_symbol->line = line;
    new_symbol->next = NULL;
    return new_symbol;
}

int insertSymbol(SymbolTable **hash_table, SymbolTable *symbol) {
    if (!hash_table || !symbol) return 0;
    
    unsigned int index = hashFunction(symbol->name);
    
    if (hash_table[index] == NULL) {
        hash_table[index] = symbol;
        return 1;
    }
    
    SymbolTable *current = hash_table[index];
    while (current->next != NULL) {
        if (strcmp(current->name, symbol->name) == 0) {
            return 0;
        }
        if (current->next == NULL) break;
        current = current->next;
    }
    
    current->next = symbol;
    return 1;
}

SymbolTable* searchSymbol(SymbolTable **hash_table, const char *name) {
    if (!hash_table || !name) return NULL;
    
    unsigned int index = hashFunction(name);
    
    SymbolTable *current = hash_table[index];
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

void destroySymbolTable(SymbolTable **hash_table) {
    if (!hash_table) return;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        SymbolTable *current = hash_table[i];
        while (current != NULL) {
            SymbolTable *next = current->next;
            free(current->name);
            free(current->category);
            free(current->type);
            free(current->scope);
            free(current->address);
            free(current->parameters);
            free(current->literal_value);
            free(current->declared_in);
            free(current->visibility);
            free(current);
            current = next;
        }
    }

    free(hash_table);
}

void clearSymbolTable(SymbolTable **hash_table) {
    if (!hash_table) return;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        SymbolTable *current = hash_table[i];
        while (current != NULL) {
            SymbolTable *next = current->next;
            free(current->name);
            free(current->category);
            free(current->type);
            free(current->scope);
            free(current->address);
            free(current->parameters);
            free(current->literal_value);
            free(current->declared_in);
            free(current->visibility);
            free(current);
            current = next;
        }
        hash_table[i] = NULL;
    }
}

char* getSymbolTable(SymbolTable **hash_table) {
    if (!hash_table) return NULL;

    size_t buffer_size = 1024; 
    char *buffer = (char *)malloc(buffer_size);
    if (!buffer) return NULL;
    buffer[0] = '\0';

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        SymbolTable *current = hash_table[i];
        while (current != NULL) {
            char entry[512];
            snprintf(entry, sizeof(entry),
                     "Name: %s, Category: %s, Type: %s, Scope: %s, Address: %s, Size: %d, Parameters: %s, Literal Value: %s, Declared In: %s, Visibility: %s, Line: %d\n",
                     current->name, current->category, current->type, current->scope,
                     current->address, current->size, current->parameters,
                     current->literal_value, current->declared_in,
                     current->visibility, current->line);

            if (strlen(buffer) + strlen(entry) + 1 > buffer_size) {
                buffer_size *= 2;
                char *new_buffer = (char *)realloc(buffer, buffer_size);
                if (!new_buffer) {
                    free(buffer);
                    return NULL;
                }
                buffer = new_buffer;
            }

            strcat(buffer, entry);
            current = current->next;
        }
    }

    return buffer;
}

static int global_temp_counter = 0;
static int global_label_counter = 0;

static char* generateTempVar() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "t%d", global_temp_counter++);
    return strdup(buffer);
}

static char* generateLabel() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "L%d", global_label_counter++);
    return strdup(buffer);
}

IntermediateCode* createIntermediateCode() {
    IntermediateCode *code = (IntermediateCode *)malloc(sizeof(IntermediateCode));
    if (!code) return NULL;

    code->instruction_capacity = 256;
    code->instructions = (TACInstruction *)malloc(sizeof(TACInstruction) * code->instruction_capacity);
    
    if (!code->instructions) {
        free(code);
        return NULL;
    }

    code->instruction_count = 0;
    code->temp_counter = 0;
    code->label_counter = 0;

    return code;
}

void addTACInstruction(IntermediateCode *code, TACOpType op, const char *result,
                       const char *arg1, const char *arg2, int line) {
    if (!code) return;

    if (code->instruction_count >= code->instruction_capacity) {
        code->instruction_capacity *= 2;
        TACInstruction *new_instructions = (TACInstruction *)realloc(code->instructions,
                                                                     sizeof(TACInstruction) * code->instruction_capacity);
        if (!new_instructions) {
            return;
        }
        code->instructions = new_instructions;
    }

    TACInstruction *instr = &code->instructions[code->instruction_count];
    instr->op = op;
    instr->result = result ? strdup(result) : NULL;
    instr->arg1 = arg1 ? strdup(arg1) : NULL;
    instr->arg2 = arg2 ? strdup(arg2) : NULL;
    instr->line = line;

    code->instruction_count++;
}

const char* tacOpToString(TACOpType op) {
    switch (op) {
        case TAC_ASSIGN:           return "ASSIGN";
        case TAC_ASSIGN_LITERAL:   return "ASSIGN_LITERAL";
        case TAC_ADD:              return "ADD";
        case TAC_SUB:              return "SUB";
        case TAC_MUL:              return "MUL";
        case TAC_DIV:              return "DIV";
        case TAC_MOD:              return "MOD";
        case TAC_LT:               return "LT";
        case TAC_GT:               return "GT";
        case TAC_LE:               return "LE";
        case TAC_GE:               return "GE";
        case TAC_EQ:               return "EQ";
        case TAC_NE:               return "NE";
        case TAC_AND:              return "AND";
        case TAC_OR:               return "OR";
        case TAC_NOT:              return "NOT";
        case TAC_LABEL:            return "LABEL";
        case TAC_JUMP:             return "JUMP";
        case TAC_JUMP_IF_FALSE:    return "JUMP_IF_FALSE";
        case TAC_JUMP_IF_TRUE:     return "JUMP_IF_TRUE";
        case TAC_CALL:             return "CALL";
        case TAC_RETURN:           return "RETURN";
        case TAC_PARAM:            return "PARAM";
        case TAC_READ:             return "READ";
        case TAC_WRITE:            return "WRITE";
        case TAC_ARRAY_ACCESS:     return "ARRAY_ACCESS";
        case TAC_ARRAY_ASSIGN:     return "ARRAY_ASSIGN";
        case TAC_FUNCTION_START:   return "FUNCTION_START";
        case TAC_FUNCTION_END:     return "FUNCTION_END";
        default:                   return "UNKNOWN";
    }
}

static char* generateCodeForNode(ASTNode *node, IntermediateCode *code,
                                SymbolTable **symtab, ErrorTable *errtab) {
    if (!node || !code) return NULL;
    if (!node->type) return NULL;

    if (strcmp(node->type, "VAR_DECL") == 0 || strcmp(node->type, "VARIABLE") == 0) {
        return NULL;
    }

    if (strcmp(node->type, "ASSIGN") == 0 || strcmp(node->type, "ASSIGNMENT") == 0) {
        if (node->child_count >= 2) {
            ASTNode *lhs = node->children[0];  
            ASTNode *rhs = node->children[1];  
            
            const char *lhs_name = lhs->value ? lhs->value : "unknown";
            
            char *rhs_result = generateCodeForNode(rhs, code, symtab, errtab);
            
            if (rhs_result) {
                addTACInstruction(code, TAC_ASSIGN, lhs_name, rhs_result, NULL, node->line);
                free(rhs_result);
            }
        }
        return NULL;
    }

    if (strcmp(node->type, "PLUS") == 0 || strcmp(node->type, "ADD_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "0";
            const char *op2 = arg2 ? arg2 : "0";
            
            addTACInstruction(code, TAC_ADD, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "MINUS") == 0 || strcmp(node->type, "SUB_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "0";
            const char *op2 = arg2 ? arg2 : "0";
            
            addTACInstruction(code, TAC_SUB, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "TIMES") == 0 || strcmp(node->type, "MUL_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "1";
            const char *op2 = arg2 ? arg2 : "1";
            
            addTACInstruction(code, TAC_MUL, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "DIVIDE") == 0 || strcmp(node->type, "DIV_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "1";
            const char *op2 = arg2 ? arg2 : "1";
            
            addTACInstruction(code, TAC_DIV, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "MOD") == 0 || strcmp(node->type, "MOD_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "1";
            const char *op2 = arg2 ? arg2 : "1";
            
            addTACInstruction(code, TAC_MOD, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "LT") == 0 || strcmp(node->type, "LT_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "0";
            const char *op2 = arg2 ? arg2 : "0";
            
            addTACInstruction(code, TAC_LT, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "GT") == 0 || strcmp(node->type, "GT_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "0";
            const char *op2 = arg2 ? arg2 : "0";
            
            addTACInstruction(code, TAC_GT, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "LE") == 0 || strcmp(node->type, "LE_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "0";
            const char *op2 = arg2 ? arg2 : "0";
            
            addTACInstruction(code, TAC_LE, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "GE") == 0 || strcmp(node->type, "GE_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "0";
            const char *op2 = arg2 ? arg2 : "0";
            
            addTACInstruction(code, TAC_GE, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "EQ") == 0 || strcmp(node->type, "EQ_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "0";
            const char *op2 = arg2 ? arg2 : "0";
            
            addTACInstruction(code, TAC_EQ, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "NE") == 0 || strcmp(node->type, "NE_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "0";
            const char *op2 = arg2 ? arg2 : "0";
            
            addTACInstruction(code, TAC_NE, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "AND") == 0 || strcmp(node->type, "AND_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "false";
            const char *op2 = arg2 ? arg2 : "false";
            
            addTACInstruction(code, TAC_AND, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "OR") == 0 || strcmp(node->type, "OR_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "false";
            const char *op2 = arg2 ? arg2 : "false";
            
            addTACInstruction(code, TAC_OR, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "NOT") == 0 || strcmp(node->type, "NOT_OP") == 0) {
        if (node->child_count >= 1) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "false";
            
            addTACInstruction(code, TAC_NOT, result, op1, NULL, node->line);
            
            if (arg1) free(arg1);
            
            return result;
        }
        return NULL;
    }

    if (strcmp(node->type, "IF") == 0 || strcmp(node->type, "IF_STMT") == 0) {
        if (node->child_count >= 2) {
            ASTNode *condition = node->children[0];
            ASTNode *then_block = node->children[1];
            ASTNode *else_block = (node->child_count >= 3) ? node->children[2] : NULL;

            char *cond_var = generateCodeForNode(condition, code, symtab, errtab);
            const char *cond = cond_var ? cond_var : "0";

            if (else_block) {
                char *else_label = generateLabel();
                char *end_label = generateLabel();
                
                addTACInstruction(code, TAC_JUMP_IF_FALSE, else_label, cond, NULL, condition->line);
                if (cond_var) free(cond_var);
                
                char *then_result = generateCodeForNode(then_block, code, symtab, errtab);
                if (then_result) free(then_result);
              
                addTACInstruction(code, TAC_JUMP, end_label, NULL, NULL, node->line);
                
                addTACInstruction(code, TAC_LABEL, else_label, NULL, NULL, node->line);
                char *else_result = generateCodeForNode(else_block, code, symtab, errtab);
                if (else_result) free(else_result);
                
                addTACInstruction(code, TAC_LABEL, end_label, NULL, NULL, node->line);
                
                free(else_label);
                free(end_label);
                
            } else {
                char *end_label = generateLabel();
                
                addTACInstruction(code, TAC_JUMP_IF_FALSE, end_label, cond, NULL, condition->line);
                if (cond_var) free(cond_var);
                
                char *then_result = generateCodeForNode(then_block, code, symtab, errtab);
                if (then_result) free(then_result);
                
                addTACInstruction(code, TAC_LABEL, end_label, NULL, NULL, node->line);
                
                free(end_label);
            }
        }
        return NULL;
    }

    if (strcmp(node->type, "WHILE") == 0 || strcmp(node->type, "WHILE_STMT") == 0) {
        if (node->child_count >= 2) {
            ASTNode *condition = node->children[0];
            ASTNode *body = node->children[1];

            char *loop_label = generateLabel();
            char *end_label = generateLabel();

            addTACInstruction(code, TAC_LABEL, loop_label, NULL, NULL, node->line);

            char *cond_var = generateCodeForNode(condition, code, symtab, errtab);

            const char *cond = cond_var ? cond_var : "0";
            addTACInstruction(code, TAC_JUMP_IF_FALSE, end_label, cond, NULL, condition->line);
            
            if (cond_var) free(cond_var);

            char *body_result = generateCodeForNode(body, code, symtab, errtab);
            if (body_result) free(body_result);

            addTACInstruction(code, TAC_JUMP, loop_label, NULL, NULL, node->line);

            addTACInstruction(code, TAC_LABEL, end_label, NULL, NULL, node->line);

            free(loop_label);
            free(end_label);
        }
        return NULL;
    }

    if (strcmp(node->type, "READ") == 0 || strcmp(node->type, "READ_STMT") == 0) {
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i]->type && strcmp(node->children[i]->type, "ARGUMENT") == 0) {
                const char *var_name = node->children[i]->value ? node->children[i]->value : "unknown";
                addTACInstruction(code, TAC_READ, var_name, NULL, NULL, node->line);
            }
        }
        return NULL;
    }

    if (strcmp(node->type, "WRITE") == 0 || strcmp(node->type, "WRITE_STMT") == 0) {
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i]->type && strcmp(node->children[i]->type, "ARGUMENT") == 0) {
                const char *var_name = node->children[i]->value ? node->children[i]->value : "unknown";
                addTACInstruction(code, TAC_WRITE, NULL, var_name, NULL, node->line);
            }
        }
        return NULL;
    }

    if (strcmp(node->type, "FUNCTION") == 0 || strcmp(node->type, "PROCEDURE") == 0 ||
        strcmp(node->type, "FUNC_DECL") == 0 || strcmp(node->type, "PROC_DECL") == 0) {
        
        const char *func_name = "unknown_function";
        ASTNode *body = NULL;
        
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i] && node->children[i]->type) {
                
                if (strcmp(node->children[i]->type, "IDENTIFIER") == 0) {
                    func_name = node->children[i]->value ? node->children[i]->value : "unknown_function";
                }
                
                else if (strcmp(node->children[i]->type, "BLOCK") == 0 ||
                        strcmp(node->children[i]->type, "COMPOUND") == 0 ||
                        strcmp(node->children[i]->type, "BEGIN") == 0 ||
                        strcmp(node->children[i]->type, "STATEMENT_LIST") == 0) {
                    body = node->children[i];
                }
            }
        }
        
        
        addTACInstruction(code, TAC_FUNCTION_START, func_name, NULL, NULL, node->line);

       
        if (body) {
            char *result = generateCodeForNode(body, code, symtab, errtab);
            if (result) free(result);
        }
        
        addTACInstruction(code, TAC_RETURN, NULL, NULL, NULL, node->line);
        addTACInstruction(code, TAC_FUNCTION_END, func_name, NULL, NULL, node->line);
        
        return NULL;
    }

    
    if (strcmp(node->type, "RETURN") == 0 || strcmp(node->type, "RETURN_STMT") == 0) {
        const char *return_val = NULL;
        if (node->child_count >= 1) {
            char *result = generateCodeForNode(node->children[0], code, symtab, errtab);
            if (result) {
                return_val = strdup(result);
                free(result);
            }
        }
        addTACInstruction(code, TAC_RETURN, return_val, NULL, NULL, node->line);
        if (return_val) free((void*)return_val);
        return NULL;
    }

    
    if (strcmp(node->type, "CALL") == 0 || strcmp(node->type, "FUNCTION_CALL") == 0) {
        const char *func_name = node->value ? node->value : "unknown_function";
        char *result = generateTempVar();
        addTACInstruction(code, TAC_CALL, result, func_name, NULL, node->line);
        return result;
    }

    if (strcmp(node->type, "IDENTIFIER") == 0 || strcmp(node->type, "ID") == 0 ||
        strcmp(node->type, "VALUE") == 0 || strcmp(node->type, "NUMBER") == 0 ||
        strcmp(node->type, "LITERAL") == 0) {
        if (node->value) {
            return strdup(node->value);
        }
        return NULL;
    }

    if (strcmp(node->type, "BLOCK") == 0 || strcmp(node->type, "COMPOUND") == 0 ||
        strcmp(node->type, "PROGRAM") == 0 || strcmp(node->type, "STATEMENT_LIST") == 0) {
        for (int i = 0; i < node->child_count; i++) {
            char *result = generateCodeForNode(node->children[i], code, symtab, errtab);
            if (result) free(result);
        }
        return NULL;
    }

    for (int i = 0; i < node->child_count; i++) {
        char *result = generateCodeForNode(node->children[i], code, symtab, errtab);
        if (result) free(result);
    }
    
    return NULL;
}

IntermediateCode* generateIntermediateCode(ASTNode *ast, SymbolTable **symtab, ErrorTable *errtab) {
    IntermediateCode *code = createIntermediateCode();
    
    if (!code) {
        return NULL;
    }

    if (!ast) {
        return code;
    }

    global_temp_counter = 0;
    global_label_counter = 0;

    generateCodeForNode(ast, code, symtab, errtab);

    return code;
}

void printIntermediateCode(IntermediateCode *code) {
    if (!code || code->instruction_count == 0) {
        printf("Nenhuma instrução de código intermediário gerada.\n");
        return;
    }

    printf("\n");
    printf("%-8s | %-20s | %-20s | %-20s | %-20s | %-6s\n",
           "ÍNDICE", "OPERAÇÃO", "RESULTADO", "ARG1", "ARG2", "LINHA");
    printf("----------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < code->instruction_count; i++) {
        TACInstruction *instr = &code->instructions[i];
        
        const char *result = instr->result ? instr->result : "-";
        const char *arg1 = instr->arg1 ? instr->arg1 : "-";
        const char *arg2 = instr->arg2 ? instr->arg2 : "-";
        
        printf("%-8d | %-20s | %-20s | %-20s | %-20s | %-6d\n",
               i,
               tacOpToString(instr->op),
               result,
               arg1,
               arg2,
               instr->line);
    }
    printf("\n");
}

int writeIntermediateCodeToFile(IntermediateCode *code, const char *filepath) {
    if (!code || !filepath) {
        return -1;
    }

    FILE *file = NULL;
    if (openFile(&file, filepath, "w")) {
        return -1;
    }

    if (!file) {
        return -1;
    }

    fprintf(file, "%-8s | %-20s | %-20s | %-20s | %-20s | %-6s\n",
            "INDICE", "OPERACAO", "RESULTADO", "ARG1", "ARG2", "LINHA");
    fprintf(file, "----------------------------------------------------------------------------------------------------------\n");

    if (code->instruction_count == 0) {
        fprintf(file, "Nenhuma instrução de código intermediário gerada.\n");
    } else {
        for (int i = 0; i < code->instruction_count; i++) {
            TACInstruction *instr = &code->instructions[i];
            
            const char *result = instr->result ? instr->result : "-";
            const char *arg1 = instr->arg1 ? instr->arg1 : "-";
            const char *arg2 = instr->arg2 ? instr->arg2 : "-";
            
            fprintf(file, "%-8d | %-20s | %-20s | %-20s | %-20s | %-6d\n",
                    i,
                    tacOpToString(instr->op),
                    result,
                    arg1,
                    arg2,
                    instr->line);
        }
    }

    closeFile(file);
    return 0;
}

void printIntermediateCodeSequential(IntermediateCode *code) {
    if (!code || code->instruction_count == 0) {
        printf("No intermediate code generated.\n");
        return;
    }

    printf("\n=== INTERMEDIATE CODE (TAC) ===\n\n");

    for (int i = 0; i < code->instruction_count; i++) {
        TACInstruction *instr = &code->instructions[i];
        
        switch (instr->op) {
            case TAC_ASSIGN:
                printf("    %s = %s\n", instr->result, instr->arg1);
                break;
                
            case TAC_ADD:
                printf("    %s = %s + %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_SUB:
                printf("    %s = %s - %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_MUL:
                printf("    %s = %s * %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_DIV:
                printf("    %s = %s div %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_MOD:
                printf("    %s = %s mod %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_LT:
                printf("    %s = %s < %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_GT:
                printf("    %s = %s > %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_LE:
                printf("    %s = %s <= %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_GE:
                printf("    %s = %s >= %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_EQ:
                printf("    %s = %s == %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_NE:
                printf("    %s = %s != %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_AND:
                printf("    %s = %s and %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_OR:
                printf("    %s = %s or %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_NOT:
                printf("    %s = not %s\n", instr->result, instr->arg1);
                break;
                
            case TAC_LABEL:
                printf("%s:\n", instr->result);
                break;
                
            case TAC_JUMP:
                printf("    goto %s\n", instr->result);
                break;
                
            case TAC_JUMP_IF_FALSE:
                printf("    if_false %s goto %s\n", instr->arg1, instr->result);
                break;
                
            case TAC_JUMP_IF_TRUE:
                printf("    if_true %s goto %s\n", instr->arg1, instr->result);
                break;
                
            case TAC_READ:
                printf("    read %s\n", instr->result);
                break;
                
            case TAC_WRITE:
                printf("    write %s\n", instr->arg1);
                break;
                
            case TAC_FUNCTION_START:
                printf("\nPROC_START %s\n", instr->result);
                break;
                
            case TAC_FUNCTION_END:
                printf("PROC_END %s\n", instr->result);
                break;
                
            case TAC_CALL:
                printf("    call %s\n", instr->arg1);
                if (instr->result) {
                    printf("    %s = return_value\n", instr->result);
                }
                break;
                
            case TAC_PARAM:
                printf("    param %s\n", instr->arg1);
                break;
                
            case TAC_RETURN:
                if (instr->result) {
                    printf("    return %s\n", instr->result);
                } else {
                    printf("    return\n");
                }
                break;
                
            case TAC_ARRAY_ACCESS:
                printf("    %s = %s[%s]\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            case TAC_ARRAY_ASSIGN:
                printf("    %s[%s] = %s\n", instr->result, instr->arg1, instr->arg2);
                break;
                
            default:
                printf("    ; Unknown operation: %s\n", tacOpToString(instr->op));
                break;
        }
    }
    
    printf("\n=== END INTERMEDIATE CODE ===\n\n");
}

int writeIntermediateCodeSequential(IntermediateCode *code, const char *filepath) {
    if (!code || !filepath) {
        return -1;
    }

    FILE *file = NULL;
    if (openFile(&file, filepath, "w")) {
        return -1;
    }

    if (!file) {
        return -1;
    }

    fprintf(file, "=== INTERMEDIATE CODE (TAC) ===\n\n");

    if (code->instruction_count == 0) {
        fprintf(file, "No intermediate code generated.\n");
    } else {
        for (int i = 0; i < code->instruction_count; i++) {
            TACInstruction *instr = &code->instructions[i];
            
            switch (instr->op) {
                case TAC_ASSIGN:
                    fprintf(file, "    %s = %s\n", instr->result, instr->arg1);
                    break;
                    
                case TAC_ADD:
                    fprintf(file, "    %s = %s + %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_SUB:
                    fprintf(file, "    %s = %s - %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_MUL:
                    fprintf(file, "    %s = %s * %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_DIV:
                    fprintf(file, "    %s = %s div %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_MOD:
                    fprintf(file, "    %s = %s mod %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_LT:
                    fprintf(file, "    %s = %s < %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_GT:
                    fprintf(file, "    %s = %s > %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_LE:
                    fprintf(file, "    %s = %s <= %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_GE:
                    fprintf(file, "    %s = %s >= %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_EQ:
                    fprintf(file, "    %s = %s == %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_NE:
                    fprintf(file, "    %s = %s != %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_AND:
                    fprintf(file, "    %s = %s and %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_OR:
                    fprintf(file, "    %s = %s or %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_NOT:
                    fprintf(file, "    %s = not %s\n", instr->result, instr->arg1);
                    break;
                    
                case TAC_LABEL:
                    fprintf(file, "%s:\n", instr->result);
                    break;
                    
                case TAC_JUMP:
                    fprintf(file, "    goto %s\n", instr->result);
                    break;
                    
                case TAC_JUMP_IF_FALSE:
                    fprintf(file, "    if_false %s goto %s\n", instr->arg1, instr->result);
                    break;
                    
                case TAC_JUMP_IF_TRUE:
                    fprintf(file, "    if_true %s goto %s\n", instr->arg1, instr->result);
                    break;
                    
                case TAC_READ:
                    fprintf(file, "    read %s\n", instr->result);
                    break;
                    
                case TAC_WRITE:
                    fprintf(file, "    write %s\n", instr->arg1);
                    break;
                    
                case TAC_FUNCTION_START:
                    fprintf(file, "\nPROC_START %s\n", instr->result);
                    break;
                    
                case TAC_FUNCTION_END:
                    fprintf(file, "PROC_END %s\n", instr->result);
                    break;
                    
                case TAC_CALL:
                    fprintf(file, "    call %s\n", instr->arg1);
                    if (instr->result) {
                        fprintf(file, "    %s = return_value\n", instr->result);
                    }
                    break;
                    
                case TAC_PARAM:
                    fprintf(file, "    param %s\n", instr->arg1);
                    break;
                    
                case TAC_RETURN:
                    if (instr->result) {
                        fprintf(file, "    return %s\n", instr->result);
                    } else {
                        fprintf(file, "    return\n");
                    }
                    break;
                    
                case TAC_ARRAY_ACCESS:
                    fprintf(file, "    %s = %s[%s]\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                case TAC_ARRAY_ASSIGN:
                    fprintf(file, "    %s[%s] = %s\n", instr->result, instr->arg1, instr->arg2);
                    break;
                    
                default:
                    fprintf(file, "    ; Unknown operation: %s\n", tacOpToString(instr->op));
                    break;
            }
        }
    }

    fprintf(file, "\n=== END INTERMEDIATE CODE ===\n");
    
    closeFile(file);
    return 0;
}

void freeIntermediateCode(IntermediateCode *code) {
    if (!code) return;

    if (code->instructions) {
        for (int i = 0; i < code->instruction_count; i++) {
            if (code->instructions[i].result) {
                free(code->instructions[i].result);
            }
            if (code->instructions[i].arg1) {
                free(code->instructions[i].arg1);
            }
            if (code->instructions[i].arg2) {
                free(code->instructions[i].arg2);
            }
        }
        free(code->instructions);
    }

    free(code);
}

#define TOKEN_ARITHMETIC    1
#define TOKEN_RELATIONAL    2
#define TOKEN_ASSIGNMENT    3
#define TOKEN_LOGICAL       4
#define TOKEN_ERROR         0

#define SAFE_REALLOC(ptr, size) do { \
    void *tmp = realloc(ptr, size); \
    if (!tmp && size > 0) { \
        fprintf(stderr, "Memory reallocation failed\n"); \
        free(ptr); \
        return NULL; \
    } \
    ptr = tmp; \
} while(0)

Token* lexicalAnalysis(const char *input, const char *filename, SymbolTable **symtab, ErrorTable *errtab) {
	if (!input || !symtab || !errtab) return NULL;

	const char *file_name = filename ? filename : "<unknown>";
	size_t cap = 64;
	size_t count = 0;
	int current_line = 1; 
	Token *tokens = (Token*)malloc(sizeof(Token) * cap);
	if (!tokens) return NULL;

	const char *p = input;
	while (*p != '\0') {
		while (isspace((unsigned char)*p)) {
			if (*p == '\n') current_line++;  
			p++;
		}
		if (*p == '\0') break;

		char buf[64];
		size_t bi = 0;

		if (isalpha((unsigned char)*p) || *p == '_') {
			while (isalnum((unsigned char)*p) || *p == '_') {
				if (bi < sizeof(buf)-1) buf[bi++] = *p;
				p++;
			}
			buf[bi] = '\0';

			int is_kw = is_keyword(buf);
			int is_dt = datatype_afn(buf);


			char lowerbuf[64];
			for (size_t ii = 0; ii <= bi && ii < sizeof(lowerbuf)-1; ++ii) {
				lowerbuf[ii] = (char)tolower((unsigned char)buf[ii]);
			}
			lowerbuf[bi] = '\0';

			const char *kw_list[] = {"program","var","procedure","begin","end","if","then","else","while","do","write","read","div","to","true","false", NULL};
			if (!is_kw) {
				for (int ki = 0; kw_list[ki] != NULL; ++ki) {
					if (strcmp(lowerbuf, kw_list[ki]) == 0) {
						is_kw = 1;
						break;
					}
				}
			}

			int is_boolean_literal = (strcmp(lowerbuf, "true") == 0 || strcmp(lowerbuf, "false") == 0);

			if (count + 2 > cap) { cap *= 2; SAFE_REALLOC(tokens, sizeof(Token) * cap); }
			Token t;
			t.line = current_line;  
			if (is_kw) {
				t.type = 11; 
			} else if (is_dt) {
				t.type = 13; 
			} else {
				t.type = 10; 
			}
			strncpy(t.lexeme, buf, sizeof(t.lexeme)-1); t.lexeme[sizeof(t.lexeme)-1] = '\0';
			tokens[count++] = t;


			if (!is_boolean_literal) {
				if (searchSymbol(symtab, buf) == NULL) {
					if (is_kw) {
						SymbolTable *sym = createSymbol(buf, "keyword", "-", "-", "-", 0, "-", "-", "-", "-", current_line);
						insertSymbol(symtab, sym);
					} else if (is_dt) {
						SymbolTable *sym = createSymbol(buf, "datatype", buf, "-", "-", 0, "-", "-", "-", "-", current_line);
						insertSymbol(symtab, sym);
					} else {
						SymbolTable *sym = createSymbol(buf, "identifier", "-", "-", "-", 0, "-", "-", "-", "-", current_line);
						insertSymbol(symtab, sym);
					}
				}
			}
			continue;
		}

		if (isdigit((unsigned char)*p)) {
			bi = 0;
			while (isdigit((unsigned char)*p) || *p == '.') {
				if (bi < sizeof(buf)-1) buf[bi++] = *p;
				p++;
			}
			buf[bi] = '\0';
			if (count + 2 > cap) { cap *= 2; SAFE_REALLOC(tokens, sizeof(Token) * cap); }
			Token t;
			t.type = 12;
			t.line = current_line; 
			strncpy(t.lexeme, buf, sizeof(t.lexeme)-1);
			t.lexeme[sizeof(t.lexeme)-1] = '\0';
			tokens[count++] = t;
			continue;
		}

		if (*p == '{' || (*p == '(' && *(p+1) == '*')) {
			const char *start = p;
			if (*p == '{') {
				p++;
				while (*p != '\0' && *p != '}') p++;
				if (*p == '}') p++;
			} else {
				p += 2;
				while (*p != '\0' && !(*p == '*' && *(p+1) == ')')) p++;
				if (*p == '*' && *(p+1) == ')') p += 2;
			}
			size_t len = p - start;
			if (len >= sizeof(buf)) len = sizeof(buf)-1;
			strncpy(buf, start, len); buf[len] = '\0';
			int ok = comment_afn(buf);
			if (!ok) {
				insertError(errtab, "lexical_comment", "Invalid comment token", LEXICAL_ERROR, current_line, file_name);
			}
			if (count + 2 > cap) { cap *= 2; SAFE_REALLOC(tokens, sizeof(Token) * cap); }
			Token t;
			t.type = 15;
			t.line = current_line;
			strncpy(t.lexeme, buf, sizeof(t.lexeme)-1);
			t.lexeme[sizeof(t.lexeme)-1] = '\0';
			tokens[count++] = t;
			continue;
		}

		bi = 0;
		buf[bi++] = *p;
		buf[bi] = '\0';

		char two[3] = {0,0,0};
		two[0] = *p; two[1] = *(p+1);
		if (*(p+1) != '\0') two[2] = '\0';
		int op_type = recognize_operator(two);
		if (op_type != TOKEN_ERROR) {
			if (count + 2 > cap) { cap *= 2; SAFE_REALLOC(tokens, sizeof(Token) * cap); }
			Token t; t.type = op_type; strncpy(t.lexeme, two, sizeof(t.lexeme)-1); t.lexeme[sizeof(t.lexeme)-1] = '\0';
			tokens[count++] = t; p += 2; continue;
		}

		op_type = recognize_operator(buf);
		if (op_type != TOKEN_ERROR) {
			if (count + 2 > cap) { cap *= 2; SAFE_REALLOC(tokens, sizeof(Token) * cap); }
			Token t; t.type = op_type; strncpy(t.lexeme, buf, sizeof(t.lexeme)-1); t.lexeme[sizeof(t.lexeme)-1] = '\0';
			tokens[count++] = t; p++; continue;
		}

		if (strchr("(),;:.", buf[0])) {
			if (count + 2 > cap) { cap *= 2; SAFE_REALLOC(tokens, sizeof(Token) * cap); }
			Token t; t.type = 14; /* delimiter */
			t.line = current_line;
			strncpy(t.lexeme, buf, sizeof(t.lexeme)-1); t.lexeme[sizeof(t.lexeme)-1] = '\0';
			tokens[count++] = t; p++; continue;
		}

		bi = 0;
		while (bi < (int)sizeof(buf)-1 && *p != '\0' && !isspace((unsigned char)*p) && !isalnum((unsigned char)*p)) {
			buf[bi++] = *p; p++;
		}
		buf[bi] = '\0';
		insertError(errtab, "lexical_unknown", "Unknown lexical token", LEXICAL_ERROR, current_line, file_name);
		if (count + 2 > cap) { cap *= 2; SAFE_REALLOC(tokens, sizeof(Token) * cap); }
		Token t;
		t.type = 0;
		t.line = current_line; 
		strncpy(t.lexeme, buf, sizeof(t.lexeme)-1);
		t.lexeme[sizeof(t.lexeme)-1] = '\0';
		tokens[count++] = t;
	}

	if (count + 1 > cap) SAFE_REALLOC(tokens, sizeof(Token) * (count+1));
	Token term; term.type = 0; term.lexeme[0] = '\0'; tokens[count++] = term;

	return tokens;
}


static void reportError(ErrorTable *errtab, int line, const char *filename, const char *fmt, ...) {
	if (!errtab) return;
	char buf[512];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	char namebuf[64];
	snprintf(namebuf, sizeof(namebuf), "semantic_line_%d", line);
	const char *file_name = filename ? filename : "<unknown>";
	insertError(errtab, namebuf, buf, SEMANTIC_ERROR, line, file_name);
}

static const char* inferExpressionType(ASTNode *node, SymbolTable **symtab, ErrorTable *errtab) {
    if (!node) return "unknown";
    if (!node->type) return "unknown";

    if (node->value) {
        if (strcmp(node->type, "INT_LITERAL") == 0) return "integer";
        if (strcmp(node->type, "REAL_LITERAL") == 0) return "real";
        if (strcmp(node->type, "BOOL_LITERAL") == 0) return "boolean";
    }

    if (strcmp(node->type, "PLUS") == 0 || strcmp(node->type, "MINUS") == 0 ||
        strcmp(node->type, "TIMES") == 0 || strcmp(node->type, "DIVIDE") == 0 ||
        strcmp(node->type, "DIV_OP") == 0 || strcmp(node->type, "MOD_OP") == 0) {
        if (node->child_count >= 2) {
            const char *lt = inferExpressionType(node->children[0], symtab, errtab);
            const char *rt = inferExpressionType(node->children[1], symtab, errtab);
            if (!lt || !rt) return "unknown";
            if (strcmp(lt, "unknown") == 0 || strcmp(rt, "unknown") == 0) return "unknown";
            
            int left_numeric = (strcmp(lt, "integer") == 0 || strcmp(lt, "real") == 0);
            int right_numeric = (strcmp(rt, "integer") == 0 || strcmp(rt, "real") == 0);
            
            if (!left_numeric || !right_numeric) {
                return "type_error";
            }
            
            if (strcmp(lt, "real") == 0 || strcmp(rt, "real") == 0) {
                return "real";
            }
            return "integer";
        }
        return "unknown";
    }
    
    if (strcmp(node->type, "LT") == 0 || strcmp(node->type, "GT") == 0 ||
        strcmp(node->type, "LE") == 0 || strcmp(node->type, "GE") == 0 ||
        strcmp(node->type, "EQ") == 0 || strcmp(node->type, "NE") == 0) {
        if (node->child_count >= 2) {
            const char *lt = inferExpressionType(node->children[0], symtab, errtab);
            const char *rt = inferExpressionType(node->children[1], symtab, errtab);
            if (!lt || !rt) return "unknown";
            if (strcmp(lt, "unknown") == 0 || strcmp(rt, "unknown") == 0) return "unknown";
            if (strcmp(lt, rt) != 0) {
                int left_numeric = (strcmp(lt, "integer") == 0 || strcmp(lt, "real") == 0);
                int right_numeric = (strcmp(rt, "integer") == 0 || strcmp(rt, "real") == 0);
                if (!left_numeric || !right_numeric) {
                    return "type_error";
                }
            }
        }
        return "boolean";
    }
    
    if (strcmp(node->type, "AND") == 0 || strcmp(node->type, "OR") == 0) {
        if (node->child_count >= 2) {
            const char *lt = inferExpressionType(node->children[0], symtab, errtab);
            const char *rt = inferExpressionType(node->children[1], symtab, errtab);
            if (!lt || !rt) return "unknown";
            if (strcmp(lt, "boolean") != 0 || strcmp(rt, "boolean") != 0) {
                return "type_error";
            }
        }
        return "boolean";
    }
    
    if (strcmp(node->type, "NOT") == 0) {
        if (node->child_count >= 1) {
            const char *t = inferExpressionType(node->children[0], symtab, errtab);
            if (t && strcmp(t, "boolean") != 0) {
                return "type_error";
            }
        }
        return "boolean";
    }

    if (node->type && strcmp(node->type, "VALUE") == 0 && node->value) {
    }
    
    if (node->type && strcmp(node->type, "LITERAL") == 0 && node->value) {
        const char *v = node->value;
        if (strcasecmp(v, "true") == 0 || strcasecmp(v, "false") == 0) return "boolean";

     
        int is_int = 1;
        int is_real = 0;
        for (int i = 0; v[i] != '\0'; ++i) {
            if (v[i] == '.') { is_real = 1; is_int = 0; continue; }
            if (i == 0 && (v[i] == '+' || v[i] == '-')) continue;
            if (!isdigit((unsigned char)v[i])) { is_int = 0; is_real = 0; break; }
        }
        if (is_int && !is_real) return "integer";
        if (is_real) return "real";
        return "unknown";
    }

    if (strstr(node->type, "IDENTIFIER") != NULL) {
        SymbolTable *s = searchSymbol(symtab, node->value);
        if (!s) {
            reportError(errtab, node->line, "<unknown>", "Uso de identificador '%s' não declarado.", node->value ? node->value : "<null>");
            return "unknown";
        }
        return s->type ? s->type : "unknown";
    }

    if (node->child_count >= 2) {
        const char *lt = inferExpressionType(node->children[0], symtab, errtab);
        const char *rt = inferExpressionType(node->children[1], symtab, errtab);
        if (!lt || !rt) return "unknown";
        if (strcmp(lt, rt) == 0) return lt;
        return "type_error";
    }

    return "unknown";
}

static void checkNode(ASTNode *node, SymbolTable **symtab, ErrorTable *errtab, const char *scope, const char *filename) {
	if (!node) return;
	if (!node->type) return;

	if (node->type && strcmp(node->type, "VAR_DECL") == 0) {
		if (node->child_count >= 1) {
			const char *found_type = NULL;
			for (int i = 0; i < node->child_count; ++i) {
				if (node->children[i] && node->children[i]->type && strcmp(node->children[i]->type, "TYPE") == 0) {
					found_type = node->children[i]->value ? node->children[i]->value : "-";
					break;
				}
			}

            for (int i = 0; i < node->child_count; ++i) {
				ASTNode *child = node->children[i];
				if (!child || !child->type) continue;
				if (strcmp(child->type, "IDENTIFIER") == 0) {
					const char *name = child->value ? child->value : "-";
					const char *type = found_type ? found_type : "-";
					SymbolTable *existing = searchSymbol(symtab, name);
					if (existing) {

						if ((existing->category && strcmp(existing->category, "identifier") == 0) ||
							(existing->category && strcmp(existing->category, "-") == 0)) {
							if (existing->category) free(existing->category);
							existing->category = strdup("variable");
							if (existing->type) free(existing->type);
							existing->type = strdup(type);
							if (existing->scope) free(existing->scope);
							existing->scope = strdup(scope);
							existing->line = child->line > 0 ? child->line : node->line;
						} else {
							reportError(errtab, child->line > 0 ? child->line : node->line, filename, "Redeclaração de '%s'.", name);
						}
					} else {
						SymbolTable *sym = createSymbol(name, "variable", type, scope, "-", 0, "-", "-", "-", "-", child->line);
						insertSymbol(symtab, sym);
					}
				}
			}
		}
	}

	if (node->type && strstr(node->type, "ASSIGN") != NULL) {
		if (node->child_count >= 2 && node->children[0] && node->children[1]) {
			ASTNode *lhs = node->children[0];
			ASTNode *rhs = node->children[1];
			const char *lt = inferExpressionType(lhs, symtab, errtab);
			const char *rt = inferExpressionType(rhs, symtab, errtab);
			if (strcmp(lt, "unknown") == 0 || strcmp(rt, "unknown") == 0) {
			} else if (strcmp(lt, "type_error") == 0 || strcmp(rt, "type_error") == 0) {
				reportError(errtab, node->line, filename, "Erro de tipo em expressão (linha %d).", node->line);
			} else if (strcmp(lt, rt) != 0) {
				reportError(errtab, node->line, filename, "Incompatibilidade de tipos na atribuição: esperado '%s', obtido '%s'.", lt, rt);
			}
		}
	}

	if (node->type && strstr(node->type, "IDENTIFIER") != NULL) {
		if (!searchSymbol(symtab, node->value)) {
			reportError(errtab, node->line, filename, "Uso de identificador '%s' não declarado.", node->value ? node->value : "<null>");
		}
	}

	
	if (node->type && (strstr(node->type, "CALL") != NULL || strstr(node->type, "FUNC_CALL") != NULL)) {
		ASTNode *nameNode = (node->child_count > 0) ? node->children[0] : NULL;
		if (nameNode && nameNode->value) {
			SymbolTable *s = searchSymbol(symtab, nameNode->value);
			if (!s) {
				reportError(errtab, node->line, filename, "Chamada de função/procedimento '%s' não declarada.", nameNode->value);
			} else {

			}
		}
	}

	for (int i = 0; i < node->child_count; ++i) {
		checkNode(node->children[i], symtab, errtab, scope, filename);
	}
}

int semanticAnalysis(ASTNode *root, SymbolTable **symtab, ErrorTable *errtab, const char *filename) {
	if (!root) return 1;

	const char *globalScope = "global";
	checkNode(root, symtab, errtab, globalScope, filename);

	if (!errtab) return 0; 
	return (errtab->count > 0) ? 1 : 0;
}

Parser createParser(Token *tokens, SymbolTable **symtab, ErrorTable *errtab) {
    Parser parser;
    parser.tokens = tokens;
    parser.current = 0;
    parser.symtab = symtab;
    parser.errtab = errtab;
    parser.error_count = 0;
    parser.root = NULL;
    return parser;
}

ASTNode* createASTNode(const char *type, const char *value, int line) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = type ? strdup(type) : NULL;
    node->value = value ? strdup(value) : NULL;
    node->line = line;
    node->child_count = 0;
    node->child_capacity = 4;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * node->child_capacity);
    if (!node->children) {
        free(node->type);
        free(node->value);
        free(node);
        return NULL;
    }
    return node;
}

void astAddChild(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;
    if (parent->child_count >= parent->child_capacity) {
        int newcap = parent->child_capacity * 2;
        ASTNode **tmp = (ASTNode**)realloc(parent->children, sizeof(ASTNode*) * newcap);
        if (!tmp) return;
        parent->children = tmp;
        parent->child_capacity = newcap;
    }
    parent->children[parent->child_count++] = child;
}

void freeAST(ASTNode *node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; ++i) {
        freeAST(node->children[i]);
    }
    free(node->children);
    free(node->type);
    free(node->value);
    free(node);
}

static void printASTNode(ASTNode *node, int indent, FILE *file) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) {
        if (file) fprintf(file, "  ");
        else printf("  ");
    }
    
    if (file) {
        fprintf(file, "├─ [%s]", node->type);
        if (node->value) fprintf(file, " = \"%s\"", node->value);
        if (node->line > 0) fprintf(file, " (line %d)", node->line);
        fprintf(file, "\n");
    } else {
        printf("├─ [%s]", node->type);
        if (node->value) printf(" = \"%s\"", node->value);
        if (node->line > 0) printf(" (line %d)", node->line);
        printf("\n");
    }
    
    for (int i = 0; i < node->child_count; i++) {
        printASTNode(node->children[i], indent + 1, file);
    }
}

void printAST(ASTNode *node) {
    if (!node) {
        printf("Nenhuma árvore sintática gerada.\n");
        return;
    }
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("ÁRVORE SINTÁTICA (AST)\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printASTNode(node, 0, NULL);
    printf("\n");
}

void printASTToFile(ASTNode *node, const char *filepath) {
    if (!node || !filepath) return;
    
    FILE *file = NULL;
    if (openFile(&file, filepath, "w")) return;
    if (!file) return;
    
    fprintf(file, "═══════════════════════════════════════════════════════════════\n");
    fprintf(file, "ÁRVORE SINTÁTICA (AST)\n");
    fprintf(file, "═══════════════════════════════════════════════════════════════\n");
    printASTNode(node, 0, file);
    
    closeFile(file);
}


static ASTNode* parseTerm(Parser *parser, int *index) {
    Token *current = &parser->tokens[*index];
    
    if (current->type == 10) {
        ASTNode *node = createASTNode("IDENTIFIER", current->lexeme, current->line);
        (*index)++;
        return node;
    }
    
    if (current->type == 12) {
        ASTNode *node = createASTNode("LITERAL", current->lexeme, current->line);
        (*index)++;
        return node;
    }
    
    if (current->type == 11) {
        if (strcmp(current->lexeme, "true") == 0 || strcmp(current->lexeme, "false") == 0) {
            ASTNode *node = createASTNode("LITERAL", current->lexeme, current->line);
            (*index)++;
            return node;
        }
    }
    
    return NULL;
}


static ASTNode* parseExpression(Parser *parser, int *index) {
    ASTNode *left = parseTerm(parser, index);
    if (!left) return NULL;
    
    while (parser->tokens[*index].type != 0 &&
           parser->tokens[*index].type != 14 &&
           strcmp(parser->tokens[*index].lexeme, ";") != 0 &&
           strcmp(parser->tokens[*index].lexeme, ")") != 0) {
        
        Token *op = &parser->tokens[*index];
        
        const char *op_type = NULL;
        
        if (op->type == 1) {
            if (strcmp(op->lexeme, "+") == 0) op_type = "PLUS";
            else if (strcmp(op->lexeme, "-") == 0) op_type = "MINUS";
            else if (strcmp(op->lexeme, "*") == 0) op_type = "TIMES";
            else if (strcmp(op->lexeme, "/") == 0) op_type = "DIVIDE";
        } 
        else if (op->type == 11 && strcmp(op->lexeme, "div") == 0) {
            op_type = "DIV_OP";
        } else if (op->type == 11 && strcmp(op->lexeme, "mod") == 0) {
            op_type = "MOD_OP";
        } 
        else if (op->type == 2) {
            if (strcmp(op->lexeme, "<") == 0) op_type = "LT";
            else if (strcmp(op->lexeme, ">") == 0) op_type = "GT";
            else if (strcmp(op->lexeme, "<=") == 0) op_type = "LE";
            else if (strcmp(op->lexeme, ">=") == 0) op_type = "GE";
            else if (strcmp(op->lexeme, "=") == 0) op_type = "EQ";
            else if (strcmp(op->lexeme, "<>") == 0) op_type = "NE";
        } 
       
        else if (op->type == 4) {
            if (strcmp(op->lexeme, "and") == 0) op_type = "AND";
            else if (strcmp(op->lexeme, "or") == 0) op_type = "OR";
        }
        
        if (!op_type) break;
        
        (*index)++;
        
        ASTNode *right = parseTerm(parser, index);
        if (!right) {
            break;
        }
        
        ASTNode *op_node = createASTNode(op_type, NULL, op->line);
        if (op_node) {
            if (op_node->line <= 0 || op_node->line > 10000) {
                op_node->line = left->line; 
            }
            astAddChild(op_node, left);
            astAddChild(op_node, right);
            left = op_node;
        }
    }
    
    return left;
}


static ASTNode* parseCondition(Parser *parser, int *index) {
    if (parser->tokens[*index].type == 14 &&
        strcmp(parser->tokens[*index].lexeme, "(") == 0) {
        (*index)++;
    }
    
    ASTNode *condition = parseExpression(parser, index);
    
    if (parser->tokens[*index].type == 14 &&
        strcmp(parser->tokens[*index].lexeme, ")") == 0) {
        (*index)++;
    }
    
    return condition;
}

static ASTNode* parseBlock(Parser *parser, int *index);

static ASTNode* parseStatement(Parser *parser, int *index) {
    Token *stmt_token = &parser->tokens[*index];
    
    if (stmt_token->type == 10) {
        ASTNode *assign = createASTNode("ASSIGN", NULL, stmt_token->line);
        if (!assign) return NULL;
        
        ASTNode *lhs = createASTNode("IDENTIFIER", stmt_token->lexeme, stmt_token->line);
        if (lhs) astAddChild(assign, lhs);
        
        (*index)++;
        
        int found_assign = 0;
        
        if (parser->tokens[*index].type == 3 ||
            (parser->tokens[*index].type != 0 && 
             strcmp(parser->tokens[*index].lexeme, ":=") == 0)) {
            (*index)++;
            found_assign = 1;
        }
        else if (parser->tokens[*index].type == 14 && 
                 strcmp(parser->tokens[*index].lexeme, ":") == 0 &&
                 parser->tokens[*index + 1].type == 2 &&
                 strcmp(parser->tokens[*index + 1].lexeme, "=") == 0) {
            (*index) += 2; 
            found_assign = 1;
        }
        
        if (found_assign) {
            ASTNode *rhs = parseExpression(parser, index);
            if (rhs) astAddChild(assign, rhs);
        }
        
        if (parser->tokens[*index].type == 14 &&
            strcmp(parser->tokens[*index].lexeme, ";") == 0) {
            (*index)++;
        }
        
        return assign;
    }
    
    if (stmt_token->type == 11 && strcmp(stmt_token->lexeme, "if") == 0) {
        (*index)++;
        
        ASTNode *ifNode = createASTNode("IF", NULL, stmt_token->line);
        if (!ifNode) return NULL;
        
        ASTNode *condition = parseCondition(parser, index);
        if (condition) astAddChild(ifNode, condition);
        

        if (parser->tokens[*index].type == 11 &&
            strcmp(parser->tokens[*index].lexeme, "then") == 0) {
            (*index)++;
        }
        
        ASTNode *thenBlock = NULL;
        if (parser->tokens[*index].type == 11 &&
            strcmp(parser->tokens[*index].lexeme, "begin") == 0) {
            thenBlock = parseBlock(parser, index);
        } else {
            thenBlock = parseStatement(parser, index);
        }
        if (thenBlock) astAddChild(ifNode, thenBlock);
        
 
        if (parser->tokens[*index].type == 11 &&
            strcmp(parser->tokens[*index].lexeme, "else") == 0) {
            (*index)++;
            
            ASTNode *elseBlock = NULL;
            if (parser->tokens[*index].type == 11 &&
                strcmp(parser->tokens[*index].lexeme, "begin") == 0) {
                elseBlock = parseBlock(parser, index);
            } else {
                elseBlock = parseStatement(parser, index);
            }
            if (elseBlock) astAddChild(ifNode, elseBlock);
        }
        
        return ifNode;
    }
    
    if (stmt_token->type == 11 && strcmp(stmt_token->lexeme, "while") == 0) {
        (*index)++;
        
        ASTNode *whileNode = createASTNode("WHILE", NULL, stmt_token->line);
        if (!whileNode) return NULL;
        
        ASTNode *condition = parseCondition(parser, index);
        if (condition) astAddChild(whileNode, condition);
        
        if (parser->tokens[*index].type == 11 &&
            strcmp(parser->tokens[*index].lexeme, "do") == 0) {
            (*index)++;
        }
        
        ASTNode *body = NULL;
        if (parser->tokens[*index].type == 11 &&
            strcmp(parser->tokens[*index].lexeme, "begin") == 0) {
            body = parseBlock(parser, index);
        } else {
            body = parseStatement(parser, index);
        }
        if (body) astAddChild(whileNode, body);
        
        return whileNode;
    }
    
    if (stmt_token->type == 11 && strcmp(stmt_token->lexeme, "write") == 0) {
        ASTNode *writeNode = createASTNode("WRITE", NULL, stmt_token->line);
        if (!writeNode) return NULL;
        
        (*index)++;
        
        if (parser->tokens[*index].type == 14 &&
            strcmp(parser->tokens[*index].lexeme, "(") == 0) {
            (*index)++;
            
            if (parser->tokens[*index].type == 10 ||
                parser->tokens[*index].type == 12) {
                ASTNode *arg = createASTNode("ARGUMENT",
                                            parser->tokens[*index].lexeme,
                                            parser->tokens[*index].line);
                if (arg) astAddChild(writeNode, arg);
                (*index)++;
            }
            
            if (parser->tokens[*index].type == 14 &&
                strcmp(parser->tokens[*index].lexeme, ")") == 0) {
                (*index)++;
            }
        }
        
        if (parser->tokens[*index].type == 14 &&
            strcmp(parser->tokens[*index].lexeme, ";") == 0) {
            (*index)++;
        }
        
        return writeNode;
    }
    
    if (stmt_token->type == 11 && strcmp(stmt_token->lexeme, "read") == 0) {
        ASTNode *readNode = createASTNode("READ", NULL, stmt_token->line);
        if (!readNode) return NULL;
        
        (*index)++;
        

        if (parser->tokens[*index].type == 14 &&
            strcmp(parser->tokens[*index].lexeme, "(") == 0) {
            (*index)++;
            
            if (parser->tokens[*index].type == 10) {
                ASTNode *arg = createASTNode("ARGUMENT",
                                            parser->tokens[*index].lexeme,
                                            parser->tokens[*index].line);
                if (arg) astAddChild(readNode, arg);
                (*index)++;
            }
            
            if (parser->tokens[*index].type == 14 &&
                strcmp(parser->tokens[*index].lexeme, ")") == 0) {
                (*index)++;
            }
        }
        
        if (parser->tokens[*index].type == 14 &&
            strcmp(parser->tokens[*index].lexeme, ";") == 0) {
            (*index)++;
        }
        
        return readNode;
    }
    
    (*index)++;
    return NULL;
}

static ASTNode* parseBlock(Parser *parser, int *index) {
    if (parser->tokens[*index].type != 11 ||
        strcmp(parser->tokens[*index].lexeme, "begin") != 0) {
        return NULL;
    }
    
    Token *begin_token = &parser->tokens[*index];
    (*index)++;
    
    ASTNode *block = createASTNode("BLOCK", NULL, begin_token->line);
    if (!block) return NULL;
    
    while (parser->tokens[*index].type != 0 &&
           !(parser->tokens[*index].type == 11 &&
             strcmp(parser->tokens[*index].lexeme, "end") == 0)) {
        
        ASTNode *stmt = parseStatement(parser, index);
        if (stmt) astAddChild(block, stmt);
    }
    
    if (parser->tokens[*index].type == 11 &&
        strcmp(parser->tokens[*index].lexeme, "end") == 0) {
        (*index)++;
    }
    
    if (parser->tokens[*index].type == 14 &&
        strcmp(parser->tokens[*index].lexeme, ";") == 0) {
        (*index)++;
    }
    
    return block;
}
int syntacticAnalysis(Parser *parser) {
    if (!parser || !parser->tokens) {
        return 1;
    }
    
    if (parser->root == NULL) {
        parser->root = createASTNode("PROGRAM", NULL, -1);
        if (!parser->root) return 1;
    }

    int i = 0;
    
    while (parser->tokens[i].type != 0 && 
           !(parser->tokens[i].type == 11 &&
             strcmp(parser->tokens[i].lexeme, "program") == 0)) {
        i++;
    }
    
    while (parser->tokens[i].type != 0) {
        Token *current = &parser->tokens[i];
        if (current->type == 11 && strcmp(current->lexeme, "var") == 0) {
            ASTNode *varDecl = createASTNode("VAR_DECL", NULL, current->line);
            if (varDecl) {
                i++;
                while (parser->tokens[i].type != 0 && 
                       strcmp(parser->tokens[i].lexeme, ":") != 0) {
                    if (parser->tokens[i].type == 10) {
                        ASTNode *idNode = createASTNode("IDENTIFIER", 
                                                       parser->tokens[i].lexeme, 
                                                       parser->tokens[i].line);
                        if (idNode) astAddChild(varDecl, idNode);
                    }
                    i++;
                }
                if (parser->tokens[i].type != 0 && 
                    strcmp(parser->tokens[i].lexeme, ":") == 0) {
                    i++;
                    if (parser->tokens[i].type == 11 ||
                        parser->tokens[i].type == 13 ||
                        parser->tokens[i].type == 10) {
                        ASTNode *typeNode = createASTNode("TYPE", 
                                                         parser->tokens[i].lexeme, 
                                                         parser->tokens[i].line);
                        if (typeNode) astAddChild(varDecl, typeNode);
                    }
                }
                astAddChild(parser->root, varDecl);
            }
        }
        
        else if (current->type == 11 && strcmp(current->lexeme, "procedure") == 0) {
            i++;
            ASTNode *procDecl = createASTNode("PROCEDURE", NULL, current->line);
            if (procDecl && parser->tokens[i].type == 10) {
                ASTNode *nameNode = createASTNode("IDENTIFIER", 
                                                 parser->tokens[i].lexeme, 
                                                 parser->tokens[i].line);
                if (nameNode) astAddChild(procDecl, nameNode);
            }
            astAddChild(parser->root, procDecl);
        }
        
        else if (current->type == 11 && strcmp(current->lexeme, "function") == 0) {
            i++;
            ASTNode *funcDecl = createASTNode("FUNCTION", NULL, current->line);
            if (funcDecl && parser->tokens[i].type == 10) {
                ASTNode *nameNode = createASTNode("IDENTIFIER", 
                                                 parser->tokens[i].lexeme, 
                                                 parser->tokens[i].line);
                if (nameNode) astAddChild(funcDecl, nameNode);
            }
            astAddChild(parser->root, funcDecl);
        }
        
        else if (current->type == 11 && strcmp(current->lexeme, "begin") == 0) {
            ASTNode *block = parseBlock(parser, &i);
            if (block) {
                astAddChild(parser->root, block);
            }
            continue; 
        }
        
        i++;
    }

    return 0;
}


