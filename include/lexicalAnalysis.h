#ifndef TOKEN_H
#define TOKEN_H

#include "../include/symbolTable.h"
#include "../include/errorHandler.h"

/** @brief Structure representing a token produced by the lexical analysis
 *
 * Fields:
 *  - type: integer code describing the token category (identifier, keyword,
 *          operator, delimiter, literal, etc.). Project-wide token constants
 *          (if any) determine the numeric mapping.
 *  - lexeme: null-terminated string holding the token's lexeme. Fixed size
 *            buffer allocated inside the Token structure (64 bytes).
 */
typedef struct Token {
    int type;
    char lexeme[64];
    int line;
} Token;

/** @brief Perform lexical analysis on an input C-style string.
 *
 * This function scans the provided input and returns a dynamically
 * allocated array of Token structures representing the token stream. The
 * calling code is responsible for freeing the returned pointer. Token
 * array termination is signaled by a token with type == 0 (TOKEN_ERROR)
 * and empty lexeme, or by a separate convention used in the project.
 *
 * @param input Null-terminated input string to tokenize.
 * @return Pointer to the first element of a dynamically allocated Token
 *         array. Caller must free() the returned pointer when done.
 */
Token* lexicalAnalysis(const char *input, SymbolTable **symtab, ErrorTable *errtab);

#endif /* TOKEN_H */