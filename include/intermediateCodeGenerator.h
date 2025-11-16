#ifndef INTERMEDIATE_CODE_GENERATOR_H
#define INTERMEDIATE_CODE_GENERATOR_H

#include "syntacticAnalysis.h"
#include "symbolTable.h"
#include "errorHandler.h"
#include <stdio.h>

/** @brief Three-Address Code (TAC) instruction types */
typedef enum {
    TAC_ASSIGN,           /* x = y */
    TAC_ASSIGN_LITERAL,   /* x = literal */
    TAC_ADD,              /* x = y + z */
    TAC_SUB,              /* x = y - z */
    TAC_MUL,              /* x = y * z */
    TAC_DIV,              /* x = y / z */
    TAC_MOD,              /* x = y mod z */
    TAC_LT,               /* x = y < z */
    TAC_GT,               /* x = y > z */
    TAC_LE,               /* x = y <= z */
    TAC_GE,               /* x = y >= z */
    TAC_EQ,               /* x = y == z */
    TAC_NE,               /* x = y != z */
    TAC_AND,              /* x = y && z */
    TAC_OR,               /* x = y || z */
    TAC_NOT,              /* x = !y */
    TAC_LABEL,            /* label: */
    TAC_JUMP,             /* goto label */
    TAC_JUMP_IF_FALSE,    /* if !x goto label */
    TAC_JUMP_IF_TRUE,     /* if x goto label */
    TAC_CALL,             /* call function(arg1, arg2, ...) */
    TAC_RETURN,           /* return x */
    TAC_PARAM,            /* param x */
    TAC_READ,             /* read x */
    TAC_WRITE,            /* write x */
    TAC_ARRAY_ACCESS,     /* x = array[y] */
    TAC_ARRAY_ASSIGN,     /* array[x] = y */
    TAC_FUNCTION_START,   /* function name: */
    TAC_FUNCTION_END      /* end function */
} TACOpType;

/** @brief Three-Address Code instruction structure */
typedef struct {
    TACOpType op;         /* Operation type */
    char *result;         /* Destination (result variable) */
    char *arg1;           /* First operand */
    char *arg2;           /* Second operand */
    int line;             /* Source line number */
} TACInstruction;

/** @brief Intermediate Code structure holding all instructions */
typedef struct {
    TACInstruction *instructions;
    int instruction_count;
    int instruction_capacity;
    int temp_counter;     /* Counter for temporary variables */
    int label_counter;    /* Counter for labels */
} IntermediateCode;

/**
 * @brief Create and initialize intermediate code structure
 * @return Pointer to initialized IntermediateCode
 */
IntermediateCode* createIntermediateCode();

/**
 * @brief Add a TAC instruction to the code
 * @param code Intermediate code structure
 * @param op Operation type
 * @param result Destination variable
 * @param arg1 First operand
 * @param arg2 Second operand
 * @param line Source line number
 */
void addTACInstruction(IntermediateCode *code, TACOpType op, const char *result,
                       const char *arg1, const char *arg2, int line);

/**
 * @brief Generate intermediate code from AST
 * @param ast Abstract Syntax Tree
 * @param symtab Symbol table
 * @param errtab Error table
 * @return Pointer to generated intermediate code
 */
IntermediateCode* generateIntermediateCode(ASTNode *ast, SymbolTable **symtab, ErrorTable *errtab);

/**
 * @brief Print intermediate code to stdout
 * @param code Intermediate code to print
 */
void printIntermediateCode(IntermediateCode *code);

/**
 * @brief Write intermediate code to file
 * @param code Intermediate code
 * @param filepath Path to output file
 * @return 0 on success, -1 on failure
 */
int writeIntermediateCodeToFile(IntermediateCode *code, const char *filepath);

/**
 * @brief Free intermediate code structure
 * @param code Intermediate code to free
 */
void freeIntermediateCode(IntermediateCode *code);

/**
 * @brief Convert TAC operation type to string representation
 * @param op Operation type
 * @return String representation of operation
 */
const char* tacOpToString(TACOpType op);

#endif /* INTERMEDIATE_CODE_GENERATOR_H */
