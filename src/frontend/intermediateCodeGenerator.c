#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "intermediateCodeGenerator.h"
#include "symbolTable.h"
#include "errorHandler.h"
#include "beautifulPrint.h"
#include "fileHandler.h"

/* Global counters for temporary variables and labels */
static int global_temp_counter = 0;
static int global_label_counter = 0;

/**
 * @brief Generate a unique temporary variable name
 * @return Dynamically allocated string with temporary variable name
 */
static char* generateTempVar() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "t%d", global_temp_counter++);
    return strdup(buffer);
}

/**
 * @brief Generate a unique label name
 * @return Dynamically allocated string with label name
 */
static char* generateLabel() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "L%d", global_label_counter++);
    return strdup(buffer);
}

/**
 * @brief Create and initialize intermediate code structure
 * @return Pointer to initialized IntermediateCode
 */
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

/**
 * @brief Add a TAC instruction to the code with reallocation if necessary
 * @param code Intermediate code structure
 * @param op Operation type
 * @param result Destination variable
 * @param arg1 First operand
 * @param arg2 Second operand
 * @param line Source line number
 */
void addTACInstruction(IntermediateCode *code, TACOpType op, const char *result,
                       const char *arg1, const char *arg2, int line) {
    if (!code) return;

    /* Reallocate if necessary */
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

/**
 * @brief Convert TAC operation type to string representation
 * @param op Operation type
 * @return String representation of operation
 */
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


/**
 * @brief Process an AST node and generate TAC instructions
 * @param node AST node to process
 * @param code Intermediate code structure
 * @param symtab Symbol table
 * @param errtab Error table
 * @return Name of the temporary/variable holding the result (must be freed by caller)
 */
static char* generateCodeForNode(ASTNode *node, IntermediateCode *code,
                                SymbolTable **symtab, ErrorTable *errtab) {
    if (!node || !code) return NULL;
    if (!node->type) return NULL;

        // DEBUG: Imprima o tipo do nó
    printf("DEBUG: Processando nó tipo='%s', value='%s', children=%d\n",
           node->type,
           node->value ? node->value : "NULL",
           node->child_count,
           node->line);
    // DEBUG: Imprima informações sobre cada filho
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]) {
            printf("  -> Filho[%d]: tipo='%s', value='%s'\n",
                   i,
                   node->children[i]->type ? node->children[i]->type : "NULL",
                   node->children[i]->value ? node->children[i]->value : "NULL");
        }
    }
    /* Variable Declaration */
    if (strcmp(node->type, "VAR_DECL") == 0 || strcmp(node->type, "VARIABLE") == 0) {
        return NULL;
    }

    /* Assignment Statement */
   
    if (strcmp(node->type, "ASSIGN") == 0 || strcmp(node->type, "ASSIGNMENT") == 0) {
        if (node->child_count >= 2) {
            ASTNode *lhs = node->children[0];
            ASTNode *rhs = node->children[1];
            
            const char *lhs_name = lhs->value ? lhs->value : "unknown";
            
            // Recursively evaluate the right-hand side expression
            char *rhs_result = generateCodeForNode(rhs, code, symtab, errtab);
            
            if (rhs_result) {
                // Assignment: lhs = rhs_result
                addTACInstruction(code, TAC_ASSIGN, lhs_name, rhs_result, NULL, node->line);
                free(rhs_result);
            }
        }
        return NULL;
    }

    /* Arithmetic Operations */
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

    /* Relational Operations */
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

    /* Logical Operations */
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

    /* Control Flow: If Statement */
    if (strcmp(node->type, "IF") == 0 || strcmp(node->type, "IF_STMT") == 0) {
        if (node->child_count >= 2) {
            ASTNode *condition = node->children[0];
            ASTNode *then_block = node->children[1];
            ASTNode *else_block = (node->child_count >= 3) ? node->children[2] : NULL;

            char *else_label = generateLabel();
            char *end_label = generateLabel();

            /* Evaluate condition */
            char *cond_var = generateCodeForNode(condition, code, symtab, errtab);

            /* Condition and jump */
            const char *cond = cond_var ? cond_var : "0";
            addTACInstruction(code, TAC_JUMP_IF_FALSE, else_label, cond, NULL, condition->line);
            
            if (cond_var) free(cond_var);

            /* Then block */
            char *then_result = generateCodeForNode(then_block, code, symtab, errtab);
            if (then_result) free(then_result);
            
            addTACInstruction(code, TAC_JUMP, end_label, NULL, NULL, node->line);

            /* Else label and block */
            addTACInstruction(code, TAC_LABEL, else_label, NULL, NULL, node->line);
            if (else_block) {
                char *else_result = generateCodeForNode(else_block, code, symtab, errtab);
                if (else_result) free(else_result);
            }

            /* End label */
            addTACInstruction(code, TAC_LABEL, end_label, NULL, NULL, node->line);

            free(else_label);
            free(end_label);
        }
        return NULL;
    }

    /* Control Flow: While Loop */
    if (strcmp(node->type, "WHILE") == 0 || strcmp(node->type, "WHILE_STMT") == 0) {
        if (node->child_count >= 2) {
            ASTNode *condition = node->children[0];
            ASTNode *body = node->children[1];

            char *loop_label = generateLabel();
            char *end_label = generateLabel();

            /* Loop label */
            addTACInstruction(code, TAC_LABEL, loop_label, NULL, NULL, node->line);

            /* Evaluate condition */
            char *cond_var = generateCodeForNode(condition, code, symtab, errtab);

            /* Condition and jump */
            const char *cond = cond_var ? cond_var : "0";
            addTACInstruction(code, TAC_JUMP_IF_FALSE, end_label, cond, NULL, condition->line);
            
            if (cond_var) free(cond_var);

            /* Loop body */
            char *body_result = generateCodeForNode(body, code, symtab, errtab);
            if (body_result) free(body_result);

            /* Jump back to condition */
            addTACInstruction(code, TAC_JUMP, loop_label, NULL, NULL, node->line);

            /* End label */
            addTACInstruction(code, TAC_LABEL, end_label, NULL, NULL, node->line);

            free(loop_label);
            free(end_label);
        }
        return NULL;
    }

    /* Read Statement */
    if (strcmp(node->type, "READ") == 0 || strcmp(node->type, "READ_STMT") == 0) {
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i]->type && strcmp(node->children[i]->type, "ARGUMENT") == 0) {
                const char *var_name = node->children[i]->value ? node->children[i]->value : "unknown";
                addTACInstruction(code, TAC_READ, var_name, NULL, NULL, node->line);
            }
        }
        return NULL;
    }

    /* Write Statement */
    if (strcmp(node->type, "WRITE") == 0 || strcmp(node->type, "WRITE_STMT") == 0) {
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i]->type && strcmp(node->children[i]->type, "ARGUMENT") == 0) {
                const char *var_name = node->children[i]->value ? node->children[i]->value : "unknown";
                addTACInstruction(code, TAC_WRITE, NULL, var_name, NULL, node->line);
            }
        }
        return NULL;
    }

    /* Function/Procedure Definition */
    if (strcmp(node->type, "FUNCTION") == 0 || strcmp(node->type, "PROCEDURE") == 0 ||
    strcmp(node->type, "FUNC_DECL") == 0 || strcmp(node->type, "PROC_DECL") == 0) {
    
    /* Get function/procedure name from first IDENTIFIER child */
    const char *func_name = "unknown_function";
    if (node->child_count > 0 && 
        node->children[0] && 
        node->children[0]->type &&
        strcmp(node->children[0]->type, "IDENTIFIER") == 0) {
        func_name = node->children[0]->value ? node->children[0]->value : "unknown_function";
    }
    
    addTACInstruction(code, TAC_FUNCTION_START, func_name, NULL, NULL, node->line);

    /* Process function body (children after name) */
    for (int i = 1; i < node->child_count; i++) {
        char *result = generateCodeForNode(node->children[i], code, symtab, errtab);
        if (result) free(result);
    }

    addTACInstruction(code, TAC_FUNCTION_END, func_name, NULL, NULL, node->line);
    return NULL;
    }

    /* Return Statement */
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

    /* Function Call */
    if (strcmp(node->type, "CALL") == 0 || strcmp(node->type, "FUNCTION_CALL") == 0) {
        const char *func_name = node->value ? node->value : "unknown_function";
        char *result = generateTempVar();
        addTACInstruction(code, TAC_CALL, result, func_name, NULL, node->line);
        return result;
    }

    /* Identifier or Literal */
    if (strcmp(node->type, "IDENTIFIER") == 0 || strcmp(node->type, "ID") == 0 ||
        strcmp(node->type, "VALUE") == 0 || strcmp(node->type, "NUMBER") == 0 ||
        strcmp(node->type, "LITERAL") == 0) {
        if (node->value) {
            return strdup(node->value);
        }
        return NULL;
    }

    /* Block/Compound Statement - process all children */
    if (strcmp(node->type, "BLOCK") == 0 || strcmp(node->type, "COMPOUND") == 0 ||
        strcmp(node->type, "PROGRAM") == 0 || strcmp(node->type, "STATEMENT_LIST") == 0) {
        for (int i = 0; i < node->child_count; i++) {
            char *result = generateCodeForNode(node->children[i], code, symtab, errtab);
            if (result) free(result);
        }
        return NULL;
    }

    /* Generic recursion for unhandled nodes */
    for (int i = 0; i < node->child_count; i++) {
        char *result = generateCodeForNode(node->children[i], code, symtab, errtab);
        if (result) free(result);
    }
    
    return NULL;
}

/**
 * @brief Generate intermediate code from AST
 * @param ast Abstract Syntax Tree
 * @param symtab Symbol table
 * @param errtab Error table
 * @return Pointer to generated intermediate code
 */
IntermediateCode* generateIntermediateCode(ASTNode *ast, SymbolTable **symtab, ErrorTable *errtab) {
    IntermediateCode *code = createIntermediateCode();
    
    if (!code) {
        return NULL;
    }

    if (!ast) {
        return code;
    }

    /* Reset global counters */
    global_temp_counter = 0;
    global_label_counter = 0;

    /* Generate TAC for the entire AST */
    generateCodeForNode(ast, code, symtab, errtab);

    return code;
}

/**
 * @brief Print intermediate code to stdout
 * @param code Intermediate code to print
 */
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

/**
 * @brief Write intermediate code to file
 * @param code Intermediate code
 * @param filepath Path to output file
 * @return 0 on success, -1 on failure
 */
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

    /* Write header */
    fprintf(file, "%-8s | %-20s | %-20s | %-20s | %-20s | %-6s\n",
            "INDICE", "OPERACAO", "RESULTADO", "ARG1", "ARG2", "LINHA");
    fprintf(file, "----------------------------------------------------------------------------------------------------------\n");

    /* Write instructions or message if empty */
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

/**
 * @brief Free intermediate code structure
 * @param code Intermediate code to free
 */
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
