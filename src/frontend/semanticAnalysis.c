
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

	if (node->value) {
		if (strcmp(node->type, "INT_LITERAL") == 0) return "integer";
		if (strcmp(node->type, "REAL_LITERAL") == 0) return "real";
		if (strcmp(node->type, "BOOL_LITERAL") == 0) return "boolean";
	}

	if (strstr(node->type, "IDENTIFIER") != NULL) {
		SymbolTable *s = searchSymbol(symtab, node->value);
		if (!s) {
			reportError(errtab, node->line, "Uso de identificador '%s' não declarado.", node->value ? node->value : "<null>");
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

	/* Declarações variáveis/proc/func: detectar redeclaração e inserir símbolo */
	if (strstr(node->type, "VAR") != NULL || strstr(node->type, "VAR_DECL") != NULL) {
		if (node->child_count >= 2 && node->children[0] && node->children[1]) {
			ASTNode *idnode = node->children[0];
			ASTNode *typenode = node->children[1];
			const char *name = idnode->value ? idnode->value : "-";
			const char *type = typenode->value ? typenode->value : "-";
			if (searchSymbol(symtab, name)) {
				reportError(errtab, node->line, filename, "Redeclaração de '%s'.", name);
			} else {
				SymbolTable *sym = createSymbol(name, "variable", type, scope, "-", 0, "-", "-", "-", "-", node->line);
				insertSymbol(symtab, sym);
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
