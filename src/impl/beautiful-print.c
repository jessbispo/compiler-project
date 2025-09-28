#include <stdio.h>
#include "../header/beautiful-print.h"

void greetings() {
    printf("---\nSEJA BEM VINDO, USUARIO!\n");
}

void error_message() {
    printf("---\n[ ERRO ] Um erro foi encontrado!\n");
}


void success_message(int message_status) {
    printf("---\n[ SUCESSO ] ");

    switch (message_status)
    {
    case 1:
        printf("O arquivo foi lido com sucesso!\n");
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

}
