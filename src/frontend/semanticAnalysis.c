
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

	/* The parser currently emits generic VALUE nodes for literals and identifiers
	 * (see syntacticAnalysis). Handle those here: inspect node->value to decide.
	 */
	if (node->type && strcmp(node->type, "VALUE") == 0 && node->value) {
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

		/* otherwise treat as identifier: look up in symbol table */
		SymbolTable *s = searchSymbol(symtab, v);
		if (!s) {
			reportError(errtab, node->line, "<unknown>", "Uso de identificador '%s' não declarado.", v ? v : "<null>");
			return "unknown";
		}
		return s->type ? s->type : "unknown";
	}

	if (strstr(node->type, "IDENTIFIER") != NULL) {
		SymbolTable *s = searchSymbol(symtab, node->value);
		if (!s) {
			reportError(errtab, node->line, "<unknown>", "Uso de identificador '%s' não declarado.", node->value ? node->value : "<null>");
			return "unknown";
		}
		return s->type ? s->type : "unknown";
	}

	/* Operadores binários (tentativa genérica):
	 * supondo que binops tenham child_count >= 2
	 */
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
