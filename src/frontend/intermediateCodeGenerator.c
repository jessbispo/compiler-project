#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "intermediateCodeGenerator.h"
#include "symbolTable.h"
#include "errorHandler.h"
#include "beautifulPrint.h"
#include "fileHandler.h"

// Contadores globais pra gerar nomes únicos de temporárias e labels
static int global_temp_counter = 0;
static int global_label_counter = 0;

// Gera uma variável temporária única (t0, t1, t2...)
static char* generateTempVar() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "t%d", global_temp_counter++);
    return strdup(buffer);
}

// Gera um label único (L0, L1, L2...)
static char* generateLabel() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "L%d", global_label_counter++);
    return strdup(buffer);
}

// Cria a estrutura que vai guardar todas as instruções de código intermediário
IntermediateCode* createIntermediateCode() {
    IntermediateCode *code = (IntermediateCode *)malloc(sizeof(IntermediateCode));
    if (!code) return NULL;

    // Começa com espaço pra 256 instruções
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

// Adiciona uma instrução TAC na lista (realoca se necessário)
void addTACInstruction(IntermediateCode *code, TACOpType op, const char *result,
                       const char *arg1, const char *arg2, int line) {
    if (!code) return;

    // Se encheu, dobra o tamanho
    if (code->instruction_count >= code->instruction_capacity) {
        code->instruction_capacity *= 2;
        TACInstruction *new_instructions = (TACInstruction *)realloc(code->instructions,
                                                                     sizeof(TACInstruction) * code->instruction_capacity);
        if (!new_instructions) {
            return;
        }
        code->instructions = new_instructions;
    }

    // Adiciona a instrução
    TACInstruction *instr = &code->instructions[code->instruction_count];
    instr->op = op;
    instr->result = result ? strdup(result) : NULL;
    instr->arg1 = arg1 ? strdup(arg1) : NULL;
    instr->arg2 = arg2 ? strdup(arg2) : NULL;
    instr->line = line;

    code->instruction_count++;
}

// Converte o tipo de operação TAC pra string (pra debug e impressão)
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

// Processa um nó da AST e gera as instruções TAC correspondentes
// Retorna o nome da variável/temporária que tem o resultado (precisa dar free depois)
static char* generateCodeForNode(ASTNode *node, IntermediateCode *code,
                                SymbolTable **symtab, ErrorTable *errtab) {
    if (!node || !code) return NULL;
    if (!node->type) return NULL;

    // Declaração de variável - não gera código, só ignora
    if (strcmp(node->type, "VAR_DECL") == 0 || strcmp(node->type, "VARIABLE") == 0) {
        return NULL;
    }

    // Atribuição (x := 5)
    if (strcmp(node->type, "ASSIGN") == 0 || strcmp(node->type, "ASSIGNMENT") == 0) {
        if (node->child_count >= 2) {
            ASTNode *lhs = node->children[0];  // Variável que recebe
            ASTNode *rhs = node->children[1];  // Expressão do lado direito
            
            const char *lhs_name = lhs->value ? lhs->value : "unknown";
            
            // Avalia o lado direito recursivamente
            char *rhs_result = generateCodeForNode(rhs, code, symtab, errtab);
            
            if (rhs_result) {
                // Gera: lhs = rhs_result
                addTACInstruction(code, TAC_ASSIGN, lhs_name, rhs_result, NULL, node->line);
                free(rhs_result);
            }
        }
        return NULL;
    }

    // Operação de soma (a + b)
    if (strcmp(node->type, "PLUS") == 0 || strcmp(node->type, "ADD_OP") == 0) {
        if (node->child_count >= 2) {
            char *arg1 = generateCodeForNode(node->children[0], code, symtab, errtab);
            char *arg2 = generateCodeForNode(node->children[1], code, symtab, errtab);
            
            char *result = generateTempVar();
            const char *op1 = arg1 ? arg1 : "0";
            const char *op2 = arg2 ? arg2 : "0";
            
            // Gera: result = op1 + op2
            addTACInstruction(code, TAC_ADD, result, op1, op2, node->line);
            
            if (arg1) free(arg1);
            if (arg2) free(arg2);
            
            return result;
        }
        return NULL;
    }

    // Operação de subtração (a - b)
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

    // Operação de multiplicação (a * b)
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

    // Operação de divisão (a / b ou a div b)
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

    // Operação módulo (a mod b)
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

    // Menor que (a < b)
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

    // Maior que (a > b)
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

    // Menor ou igual (a <= b)
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

    // Maior ou igual (a >= b)
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

    // Igual (a == b ou a = b)
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

    // Diferente (a != b ou a <> b)
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

    // Operador AND lógico
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

    // Operador OR lógico
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

    // Operador NOT lógico
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

    // Comando IF (if condição then ... else ...)
    if (strcmp(node->type, "IF") == 0 || strcmp(node->type, "IF_STMT") == 0) {
        if (node->child_count >= 2) {
            ASTNode *condition = node->children[0];
            ASTNode *then_block = node->children[1];
            ASTNode *else_block = (node->child_count >= 3) ? node->children[2] : NULL;

            // Avalia a condição
            char *cond_var = generateCodeForNode(condition, code, symtab, errtab);
            const char *cond = cond_var ? cond_var : "0";

            if (else_block) {
                // IF com ELSE - precisa de 2 labels
                char *else_label = generateLabel();
                char *end_label = generateLabel();
                
                // Se condição for falsa, pula pro else
                addTACInstruction(code, TAC_JUMP_IF_FALSE, else_label, cond, NULL, condition->line);
                if (cond_var) free(cond_var);
                
                // Bloco then
                char *then_result = generateCodeForNode(then_block, code, symtab, errtab);
                if (then_result) free(then_result);
                
                // Pula pro fim (evita executar o else)
                addTACInstruction(code, TAC_JUMP, end_label, NULL, NULL, node->line);
                
                // Label do else
                addTACInstruction(code, TAC_LABEL, else_label, NULL, NULL, node->line);
                char *else_result = generateCodeForNode(else_block, code, symtab, errtab);
                if (else_result) free(else_result);
                
                // Label do fim
                addTACInstruction(code, TAC_LABEL, end_label, NULL, NULL, node->line);
                
                free(else_label);
                free(end_label);
                
            } else {
                // IF sem ELSE - precisa de só 1 label
                char *end_label = generateLabel();
                
                // Se condição for falsa, pula pro fim
                addTACInstruction(code, TAC_JUMP_IF_FALSE, end_label, cond, NULL, condition->line);
                if (cond_var) free(cond_var);
                
                // Bloco then
                char *then_result = generateCodeForNode(then_block, code, symtab, errtab);
                if (then_result) free(then_result);
                
                // Label do fim
                addTACInstruction(code, TAC_LABEL, end_label, NULL, NULL, node->line);
                
                free(end_label);
            }
        }
        return NULL;
    }

    // Loop WHILE (while condição do ...)
    if (strcmp(node->type, "WHILE") == 0 || strcmp(node->type, "WHILE_STMT") == 0) {
        if (node->child_count >= 2) {
            ASTNode *condition = node->children[0];
            ASTNode *body = node->children[1];

            char *loop_label = generateLabel();
            char *end_label = generateLabel();

            // Label do início do loop
            addTACInstruction(code, TAC_LABEL, loop_label, NULL, NULL, node->line);

            // Avalia a condição
            char *cond_var = generateCodeForNode(condition, code, symtab, errtab);

            // Se condição for falsa, sai do loop
            const char *cond = cond_var ? cond_var : "0";
            addTACInstruction(code, TAC_JUMP_IF_FALSE, end_label, cond, NULL, condition->line);
            
            if (cond_var) free(cond_var);

            // Corpo do loop
            char *body_result = generateCodeForNode(body, code, symtab, errtab);
            if (body_result) free(body_result);

            // Volta pro início do loop
            addTACInstruction(code, TAC_JUMP, loop_label, NULL, NULL, node->line);

            // Label do fim do loop
            addTACInstruction(code, TAC_LABEL, end_label, NULL, NULL, node->line);

            free(loop_label);
            free(end_label);
        }
        return NULL;
    }

    // Comando READ (leitura de variável)
    if (strcmp(node->type, "READ") == 0 || strcmp(node->type, "READ_STMT") == 0) {
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i]->type && strcmp(node->children[i]->type, "ARGUMENT") == 0) {
                const char *var_name = node->children[i]->value ? node->children[i]->value : "unknown";
                addTACInstruction(code, TAC_READ, var_name, NULL, NULL, node->line);
            }
        }
        return NULL;
    }

    // Comando WRITE (impressão de variável)
    if (strcmp(node->type, "WRITE") == 0 || strcmp(node->type, "WRITE_STMT") == 0) {
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i]->type && strcmp(node->children[i]->type, "ARGUMENT") == 0) {
                const char *var_name = node->children[i]->value ? node->children[i]->value : "unknown";
                addTACInstruction(code, TAC_WRITE, NULL, var_name, NULL, node->line);
            }
        }
        return NULL;
    }

    // Declaração de função/procedimento
    if (strcmp(node->type, "FUNCTION") == 0 || strcmp(node->type, "PROCEDURE") == 0 ||
        strcmp(node->type, "FUNC_DECL") == 0 || strcmp(node->type, "PROC_DECL") == 0) {
        
        const char *func_name = "unknown_function";
        ASTNode *body = NULL;
        
        // Procura o nome e o corpo do procedimento nos filhos
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i] && node->children[i]->type) {
                // Acha o identificador (nome)
                if (strcmp(node->children[i]->type, "IDENTIFIER") == 0) {
                    func_name = node->children[i]->value ? node->children[i]->value : "unknown_function";
                }
                // Acha o corpo (pode ter vários nomes diferentes)
                else if (strcmp(node->children[i]->type, "BLOCK") == 0 ||
                        strcmp(node->children[i]->type, "COMPOUND") == 0 ||
                        strcmp(node->children[i]->type, "BEGIN") == 0 ||
                        strcmp(node->children[i]->type, "STATEMENT_LIST") == 0) {
                    body = node->children[i];
                }
            }
        }
        
        // Marca início do procedimento
        addTACInstruction(code, TAC_FUNCTION_START, func_name, NULL, NULL, node->line);

        // Processa o corpo
        if (body) {
            char *result = generateCodeForNode(body, code, symtab, errtab);
            if (result) free(result);
        }
        
        // Adiciona return e marca fim do procedimento
        addTACInstruction(code, TAC_RETURN, NULL, NULL, NULL, node->line);
        addTACInstruction(code, TAC_FUNCTION_END, func_name, NULL, NULL, node->line);
        
        return NULL;
    }

    // Comando RETURN
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

    // Chamada de função
    if (strcmp(node->type, "CALL") == 0 || strcmp(node->type, "FUNCTION_CALL") == 0) {
        const char *func_name = node->value ? node->value : "unknown_function";
        char *result = generateTempVar();
        addTACInstruction(code, TAC_CALL, result, func_name, NULL, node->line);
        return result;
    }

    // Identificador ou literal - só retorna o valor
    if (strcmp(node->type, "IDENTIFIER") == 0 || strcmp(node->type, "ID") == 0 ||
        strcmp(node->type, "VALUE") == 0 || strcmp(node->type, "NUMBER") == 0 ||
        strcmp(node->type, "LITERAL") == 0) {
        if (node->value) {
            return strdup(node->value);
        }
        return NULL;
    }

    // Bloco de código - processa todos os filhos
    if (strcmp(node->type, "BLOCK") == 0 || strcmp(node->type, "COMPOUND") == 0 ||
        strcmp(node->type, "PROGRAM") == 0 || strcmp(node->type, "STATEMENT_LIST") == 0) {
        for (int i = 0; i < node->child_count; i++) {
            char *result = generateCodeForNode(node->children[i], code, symtab, errtab);
            if (result) free(result);
        }
        return NULL;
    }

    // Caso genérico - processa filhos recursivamente
    for (int i = 0; i < node->child_count; i++) {
        char *result = generateCodeForNode(node->children[i], code, symtab, errtab);
        if (result) free(result);
    }
    
    return NULL;
}

// Função principal - gera todo o código intermediário a partir da AST
IntermediateCode* generateIntermediateCode(ASTNode *ast, SymbolTable **symtab, ErrorTable *errtab) {
    IntermediateCode *code = createIntermediateCode();
    
    if (!code) {
        return NULL;
    }

    if (!ast) {
        return code;
    }

    // Reseta os contadores globais
    global_temp_counter = 0;
    global_label_counter = 0;

    // Gera TAC pra AST inteira
    generateCodeForNode(ast, code, symtab, errtab);

    return code;
}

// Imprime o código intermediário em formato de tabela
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

// Salva o código intermediário em arquivo (formato tabela)
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

    // Cabeçalho da tabela
    fprintf(file, "%-8s | %-20s | %-20s | %-20s | %-20s | %-6s\n",
            "INDICE", "OPERACAO", "RESULTADO", "ARG1", "ARG2", "LINHA");
    fprintf(file, "----------------------------------------------------------------------------------------------------------\n");

    // Escreve as instruções
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

// Imprime o código intermediário em formato sequencial (tipo assembly)
void printIntermediateCodeSequential(IntermediateCode *code) {
    if (!code || code->instruction_count == 0) {
        printf("No intermediate code generated.\n");
        return;
    }

    printf("\n=== INTERMEDIATE CODE (TAC) ===\n\n");

    for (int i = 0; i < code->instruction_count; i++) {
        TACInstruction *instr = &code->instructions[i];
        
        // Imprime cada tipo de instrução no formato apropriado
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

// Salva o código intermediário em arquivo (formato sequencial)
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
            
            // Mesma lógica do print, mas escrevendo no arquivo
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

// Libera toda a memória usada pelo código intermediário
void freeIntermediateCode(IntermediateCode *code) {
    if (!code) return;

    if (code->instructions) {
        // Libera cada string de cada instrução
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