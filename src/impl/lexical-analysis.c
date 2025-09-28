#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header/file-reader.h"
#include "../header/automata.h"
#include "../header/lexical-analysis.h"

Token* create_token(const char *lexeme, int line, int column) {
    Token *new_token = (Token *)malloc(sizeof(Token));
    if (!new_token) {
        fprintf(stderr, "Erro ao alocar memória para o token.\n");
        exit(EXIT_FAILURE);
    }
    new_token->lexeme = strdup(lexeme);
    new_token->position.line = line;
    new_token->position.column = column;
    return new_token;
}

void add_token_to_table(TokensTable *table, Token *token) {
    table->tokens = (Token **)realloc(table->tokens, sizeof(Token *) * (table->count + 1));
    if (!table->tokens) {
        fprintf(stderr, "Erro ao realocar memória para a tabela de tokens.\n");
        exit(EXIT_FAILURE);
    }
    table->tokens[table->count++] = token;
}

void free_token(Token *token) {
    if (token) {
        free(token->lexeme);
        free(token);
    }
}

void free_all_tokens(Token **tokens, int count) {
    for (int i = 0; i < count; i++) {
        free_token(tokens[i]);
    }
    free(tokens);
}

void analyze_file_content(char *file_content, int file_size, TokensTable *table) {
    Token **tokens = NULL;
    int token_count = 0;
    Position pos = {1, 1};

    for (int i = 0; i < file_size; i++) {
        if (file_content[i] == '/') {
            Token comment_token = comments_automaton(file_content, &i, &pos.line, &pos.column, &tokens, &token_count);
            if (comment_token.lexeme != NULL) {
                add_token_to_table(table, &comment_token);
            }
        }
    }

}

Token comments_automaton(char *lexeme, int *index, int *line, int *column, Token ***tokens, int *token_count) {

    State *q0 = create_state(0, 0);
    State *q1 = create_state(1, 0);
    State *q2 = create_state(2, 1);
    State *q3 = create_state(3, 0);
    State *q4 = create_state(4, 1);

    add_transition(q0, '/', q1);
    add_transition(q1, '*', q2);
    add_transition(q2, '*', q3);
    add_transition(q3, '/', q4);

    Automaton *automaton = create_automaton();
    add_state(automaton, q0);
    add_state(automaton, q1);
    add_state(automaton, q2);
    add_state(automaton, q3);
    add_state(automaton, q4);

    set_start_state(automaton, q0);
    add_final_state(automaton, q4);

    int start_index = *index;
    int start_line = *line;
    int start_column = *column;
    int current_index = start_index;
    int current_line = start_line;
    int current_column = start_column;
    char current_char = lexeme[current_index];

    Token *new_token = NULL;

    while (current_char != '\0') {
        if (process_input(automaton, &lexeme[start_index])) {
            int length = current_index - start_index + 1;
            char *comment_lexeme = (char *)malloc(length + 1);
            strncpy(comment_lexeme, &lexeme[start_index], length);
            comment_lexeme[length] = '\0';

            new_token = create_token(comment_lexeme, start_line, start_column);
            (*tokens) = (Token **)realloc(*tokens, sizeof(Token *) * (*token_count + 1));
            (*tokens)[(*token_count)++] = new_token;

            free(comment_lexeme);
            *index = current_index + 1;
            *line = current_line;
            *column = current_column + length;
            break;
        }

        if (current_char == '\n') {
            current_line++;
            current_column = 0;
        } else {
            current_column++;
        }
        current_index++;
        current_char = lexeme[current_index];
    }

    return *new_token;

}


