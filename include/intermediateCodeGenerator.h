/** @brief Módulo responsável pela geração de código intermediário em MiniPascal.
 *  @details Este módulo implementa a geração de código intermediário (Three-Address Code - TAC) a partir da Árvore Sintática Abstrata (AST) do código-fonte MiniPascal.
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862)
 */

#ifndef INTERMEDIATE_CODE_GENERATOR_H
#define INTERMEDIATE_CODE_GENERATOR_H

#include "syntacticAnalysis.h"
#include "symbolTable.h"
#include "errorHandler.h"
#include <stdio.h>

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

#endif /* INTERMEDIATE_CODE_GENERATOR_H */
