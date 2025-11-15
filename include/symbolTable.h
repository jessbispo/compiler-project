#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define HASH_TABLE_SIZE 211 // Número primo para melhor distribuição

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

#endif /* SYMBOL_TABLE_H */