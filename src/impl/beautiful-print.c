#include <stdio.h>
#include "../header/beautiful-print.h"
#include "../header/lexical-analysis.h"
#include "../header/automata.h"

void greetings() {
    printf("---\nSEJA BEM VINDO, USUARIO!\n");
}

void error_message() {
    printf("---\nUM ERRO FOI ENCONTRADO!\n");
}


void success_message(int message_status) {
    printf("---\nOPERAÇÃO BEM SUCEDIDA: ");

    switch (message_status)
    {
    case 1:
        printf("O ARQUIVO FOI LIDO COM SUCESSO!\n");
        break;
    
    default:
        break;
    }
}

void show_file_content(char *filepath, char *content, size_t size) {
    printf("- Diretório: %s\n", filepath);

    printf("- Arquivo: ");
    for (int i = size; filepath[i] != '/'; i--) {
        printf("%c", filepath[i]);
    }

    printf("\n- Tamanho do Arquivo: %zu bytes\n", size);
    
    printf("- Conteudo do Arquivo:\n");

    printf("  ");
    for (int i = 0; content[i] != '\0'; i++) {
        printf("%c", content[i]);
        if (content[i] == '\n' && content[i+1] != '\0') {
            printf("  ");
        }
    }
    printf("\n");

}

void lexical_analysis_message() {
    printf("---\nANÁLISE LÉXICA\n");
}

void print_token_table(TokensTable *table) {
    printf("---\nTABELA DE TOKENS\n");
    
    if (table == NULL || table->count == 0) {
        printf("Nenhum token identificado.\n");
        return;
    }

    for (int i = 0; i < table->count; i++) {
        Token *token = table->tokens[i];
        if (token != NULL) {
            printf("Token %d: Lexema='%s', Linha=%d, Coluna=%d\n", 
                   i + 1, 
                   token->lexeme, 
                   token->position.line, 
                   token->position.column);
        }
    }
    
}