#include <stdio.h>
#include <stdlib.h>

#include "./header/file-reader.h"
#include "./header/beautiful-print.h"
#include "./header/automata.h"
#include "./header/lexical-analysis.h"

int main(int argc, char *argv[]) {

    char *filepath;
    char *file_content;
    char *buffer;
    size_t file_size;

    TokensTable *table;

    int read_file_result;

    greetings();

    if (argc < 2) {
        printf(">> insira o caminho do arquivo: ");
        scanf("%s", filepath);
    } else {

        // TODO: corrigir o Segmentation fault (core dumped)
        filepath = argv[0];
        file_content = argv[1];
        file_size = atoi(argv[2]);

        printf(">> arquivo inserido: %s\n", filepath);
        printf(">> conteúdo do arquivo: %s\n", file_content);
        printf(">> tamanho do arquivo: %zu\n", file_size);

        return 0;
    }

    read_file_result = read_file(filepath, &file_content, &file_size);

    if (read_file_result != 0) {
        error_message();
        fprintf(stderr, "Especifição: não foi possível ler o arquivo inserido: %s\n", filepath);
        return EXIT_FAILURE;
    }

    success_message(1);
    show_file_content(filepath, file_content, file_size);

    lexical_analysis_message();

    // Realizar a analise léxica para os comentários, variáveis, operadores, delimitadores e palavras reservadas do MiniPascal

    analyze_file_content(file_content, file_size, table);
    print_token_table();
    
    free(file_content);
    return EXIT_SUCCESS;
}