
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "syntacticAnalysis.h"
#include "symbolTable.h"
#include "errorHandler.h"
#include "semanticAnalysis.h"

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

/* Inferir tipo de expressão de forma simples: procura literais e identificadores
 * Retorna string estática como "integer", "real", "boolean", "unknown" ou "type_error"
 */
static const char* inferExpressionType(ASTNode *node, SymbolTable **symtab, ErrorTable *errtab) {
    if (!node) return "unknown";
    if (!node->type) return "unknown";

    /* Handle explicit literal-type nodes first */
    if (node->value) {
        if (strcmp(node->type, "INT_LITERAL") == 0) return "integer";
        if (strcmp(node->type, "REAL_LITERAL") == 0) return "real";
        if (strcmp(node->type, "BOOL_LITERAL") == 0) return "boolean";
    }

    /* Handle operators explicitly */
    if (strcmp(node->type, "PLUS") == 0 || strcmp(node->type, "MINUS") == 0 ||
        strcmp(node->type, "TIMES") == 0 || strcmp(node->type, "DIVIDE") == 0 ||
        strcmp(node->type, "DIV_OP") == 0 || strcmp(node->type, "MOD_OP") == 0) {
        /* Arithmetic operators: infer type from operands */
        if (node->child_count >= 2) {
            const char *lt = inferExpressionType(node->children[0], symtab, errtab);
            const char *rt = inferExpressionType(node->children[1], symtab, errtab);
            if (!lt || !rt) return "unknown";
            if (strcmp(lt, "unknown") == 0 || strcmp(rt, "unknown") == 0) return "unknown";
            
            /* Both operands must be numeric */
            int left_numeric = (strcmp(lt, "integer") == 0 || strcmp(lt, "real") == 0);
            int right_numeric = (strcmp(rt, "integer") == 0 || strcmp(rt, "real") == 0);
            
            if (!left_numeric || !right_numeric) {
                return "type_error";
            }
            
            /* If either is real, result is real; otherwise integer */
            if (strcmp(lt, "real") == 0 || strcmp(rt, "real") == 0) {
                return "real";
            }
            return "integer";
        }
        return "unknown";
    }
    
    /* Relational operators: return boolean */
    if (strcmp(node->type, "LT") == 0 || strcmp(node->type, "GT") == 0 ||
        strcmp(node->type, "LE") == 0 || strcmp(node->type, "GE") == 0 ||
        strcmp(node->type, "EQ") == 0 || strcmp(node->type, "NE") == 0) {
        /* Just verify operands are valid, result is always boolean */
        if (node->child_count >= 2) {
            const char *lt = inferExpressionType(node->children[0], symtab, errtab);
            const char *rt = inferExpressionType(node->children[1], symtab, errtab);
            if (!lt || !rt) return "unknown";
            if (strcmp(lt, "unknown") == 0 || strcmp(rt, "unknown") == 0) return "unknown";
            /* Operands should be comparable (same type or both numeric) */
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
    
    /* Logical operators: operands and result are boolean */
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

    /* The parser currently emits generic VALUE nodes for literals and identifiers */
    if (node->type && strcmp(node->type, "VALUE") == 0 && node->value) {
        // ... código existente ...
    }
    
    /* Handle LITERAL nodes */
    if (node->type && strcmp(node->type, "LITERAL") == 0 && node->value) {
        const char *v = node->value;
        /* boolean literal */
        if (strcasecmp(v, "true") == 0 || strcasecmp(v, "false") == 0) return "boolean";

        /* detect numeric: integer or real */
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

    /* Generic fallback for other binary operations */
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

	/* Declarações variáveis/proc/func: detectar redeclaração e inserir símbolo
	 * Suporte para VAR_DECL contendo múltiplos IDENTIFIERs e um nó TYPE. */
	if (node->type && strcmp(node->type, "VAR_DECL") == 0) {
		if (node->child_count >= 1) {
			/* procurar nó TYPE entre os filhos */
			const char *found_type = NULL;
			for (int i = 0; i < node->child_count; ++i) {
				if (node->children[i] && node->children[i]->type && strcmp(node->children[i]->type, "TYPE") == 0) {
					found_type = node->children[i]->value ? node->children[i]->value : "-";
					break;
				}
			}

			/* Para cada IDENTIFIER filho, inserir/atualizar símbolo usando found_type */
			for (int i = 0; i < node->child_count; ++i) {
				ASTNode *child = node->children[i];
				if (!child || !child->type) continue;
				if (strcmp(child->type, "IDENTIFIER") == 0) {
					const char *name = child->value ? child->value : "-";
					const char *type = found_type ? found_type : "-";
					SymbolTable *existing = searchSymbol(symtab, name);
					if (existing) {
						/* Se o símbolo existe mas foi inserido pela análise léxica como
						 * um placeholder (categoria "identifier" ou valor padrão "-"),
						 * atualizamos seus campos em vez de reportar redeclaração.
						 */
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

	/* Atribuições: verificar compatibilidade de tipos */
	if (node->type && strstr(node->type, "ASSIGN") != NULL) {
		if (node->child_count >= 2 && node->children[0] && node->children[1]) {
			ASTNode *lhs = node->children[0];
			ASTNode *rhs = node->children[1];
			const char *lt = inferExpressionType(lhs, symtab, errtab);
			const char *rt = inferExpressionType(rhs, symtab, errtab);
			if (strcmp(lt, "unknown") == 0 || strcmp(rt, "unknown") == 0) {
				/* já reportado por inferExpressionType */
			} else if (strcmp(lt, "type_error") == 0 || strcmp(rt, "type_error") == 0) {
				reportError(errtab, node->line, filename, "Erro de tipo em expressão (linha %d).", node->line);
			} else if (strcmp(lt, rt) != 0) {
				reportError(errtab, node->line, filename, "Incompatibilidade de tipos na atribuição: esperado '%s', obtido '%s'.", lt, rt);
			}
		}
	}

	/* Uso de identificador isolado */
	if (node->type && strstr(node->type, "IDENTIFIER") != NULL) {
		if (!searchSymbol(symtab, node->value)) {
			reportError(errtab, node->line, filename, "Uso de identificador '%s' não declarado.", node->value ? node->value : "<null>");
		}
	}

	/* Verificar chamadas de funções (checar existência e quantidade/rótulos de parâmetros)
	 * Este projeto não tem um formato padronizado de nó CALL — implementamos um tratamento genérico:
	 */
	if (node->type && (strstr(node->type, "CALL") != NULL || strstr(node->type, "FUNC_CALL") != NULL)) {
		ASTNode *nameNode = (node->child_count > 0) ? node->children[0] : NULL;
		if (nameNode && nameNode->value) {
			SymbolTable *s = searchSymbol(symtab, nameNode->value);
			if (!s) {
				reportError(errtab, node->line, filename, "Chamada de função/procedimento '%s' não declarada.", nameNode->value);
			} else {
				/* Se a tabela de símbolos armazenar parâmetros, poderíamos comparar aqui.
				 * A coluna parameters na SymbolTable existe, mas o formato não é padronizado no projeto,
				 * então apenas deixamos o gancho para futuras verificações.
				 */
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

	if (!errtab) return 0; /* nada para checar */
	return (errtab->count > 0) ? 1 : 0;
}
