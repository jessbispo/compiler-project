#include <stdio.h>
#include "../../include/lexicalAnalysis.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../include/symbolTable.h"
#include "../../include/errorHandler.h"

int identifier_afn(const char *input);
int recognize_operator(const char *input);
int delimiter_afn(const char *input);
int datatype_afn(const char *input);
int comment_afn(const char *input);
int is_keyword(const char *input);

#define TOKEN_ARITHMETIC    1
#define TOKEN_RELATIONAL    2
#define TOKEN_ASSIGNMENT    3
#define TOKEN_LOGICAL       4
#define TOKEN_ERROR         0

Token* lexicalAnalysis(const char *input, SymbolTable **symtab, ErrorTable *errtab) {
	if (!input || !symtab || !errtab) return NULL;

	size_t cap = 64;
	size_t count = 0;
	int current_line = 1;  // Initialize line counter
	Token *tokens = (Token*)malloc(sizeof(Token) * cap);
	if (!tokens) return NULL;

	const char *p = input;
	while (*p != '\0') {
		while (isspace((unsigned char)*p)) {
			if (*p == '\n') current_line++;  // Increment line counter on newline
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

			/* Fallback keyword list (case-insensitive) for keywords not
			 * recognized by the AFN or to make matching robust. We also
			 * treat boolean literals separately. */
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

			/* boolean literals should not be inserted as identifiers */
			int is_boolean_literal = (strcmp(lowerbuf, "true") == 0 || strcmp(lowerbuf, "false") == 0);

			if (count + 2 > cap) { cap *= 2; tokens = realloc(tokens, sizeof(Token) * cap); }
			Token t;
			t.line = current_line;  // Set the line number
			if (is_kw) {
				t.type = 11; /* keyword */
			} else if (is_dt) {
				t.type = 13; /* datatype */
			} else {
				t.type = 10; /* identifier */
			}
			strncpy(t.lexeme, buf, sizeof(t.lexeme)-1); t.lexeme[sizeof(t.lexeme)-1] = '\0';
			tokens[count++] = t;

			/* Insert into symbol table: datatypes and identifiers (but not general keywords)
			 * - datatypes should be stored with category "datatype"
			 * - identifiers with category "identifier"
			 */
			if (!is_boolean_literal) {
				if (searchSymbol(symtab, buf) == NULL) {
					if (is_kw) {
						SymbolTable *sym = createSymbol(buf, "keyword", "-", "-", "-", 0, "-", "-", "-", "-", current_line);
						insertSymbol(symtab, sym);
					} else if (is_dt) {
						/* For datatypes we can store the type itself */
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
			if (count + 2 > cap) { cap *= 2; tokens = realloc(tokens, sizeof(Token) * cap); }
			Token t;
			t.type = 12;
			t.line = current_line;  // Set the line number
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
				insertError(errtab, "lexical_comment", "Invalid comment token", LEXICAL_ERROR);
			}
			if (count + 2 > cap) { cap *= 2; tokens = realloc(tokens, sizeof(Token) * cap); }
			Token t;
			t.type = 15;
			t.line = current_line;  // Set the line number
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
			if (count + 2 > cap) { cap *= 2; tokens = realloc(tokens, sizeof(Token) * cap); }
			Token t; t.type = op_type; strncpy(t.lexeme, two, sizeof(t.lexeme)-1); t.lexeme[sizeof(t.lexeme)-1] = '\0';
			tokens[count++] = t; p += 2; continue;
		}

		op_type = recognize_operator(buf);
		if (op_type != TOKEN_ERROR) {
			if (count + 2 > cap) { cap *= 2; tokens = realloc(tokens, sizeof(Token) * cap); }
			Token t; t.type = op_type; strncpy(t.lexeme, buf, sizeof(t.lexeme)-1); t.lexeme[sizeof(t.lexeme)-1] = '\0';
			tokens[count++] = t; p++; continue;
		}

		bi = 0;
		/* Stop if we reach end of input or encounter space/alnum */
		while (bi < (int)sizeof(buf)-1 && *p != '\0' && !isspace((unsigned char)*p) && !isalnum((unsigned char)*p)) {
			buf[bi++] = *p; p++;
		}
		buf[bi] = '\0';
		insertError(errtab, "lexical_unknown", "Unknown lexical token", LEXICAL_ERROR);
		if (count + 2 > cap) { cap *= 2; tokens = realloc(tokens, sizeof(Token) * cap); }
		Token t;
		t.type = 0;
		t.line = current_line;  // Set the line number
		strncpy(t.lexeme, buf, sizeof(t.lexeme)-1);
		t.lexeme[sizeof(t.lexeme)-1] = '\0';
		tokens[count++] = t;
	}

	if (count + 1 > cap) tokens = realloc(tokens, sizeof(Token) * (count+1));
	Token term; term.type = 0; term.lexeme[0] = '\0'; tokens[count++] = term;

	return tokens;
}