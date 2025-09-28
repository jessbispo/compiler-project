#include <stdio.h>
#include <stdlib.h>

#include "./header/file-reader.h"
#include "./header/beautiful-print.h"

int main(int argc, char *argv[]) {

    char *filepath;
    char *file_content;
    char *buffer;
    size_t file_size;

    int read_file_result;

    greetings();

    if (argc < 2) {
        printf(">> insira o caminho do arquivo: ");
        scanf("%s", filepath);
    } else {
        filepath = argv[0];
        file_content = argv[1];
        buffer = (char *)malloc(1024 * sizeof(char));
    }

    read_file_result = read_file(filepath, &file_content, &file_size);

    if (read_file_result != 0) {
        error_message();
        fprintf(stderr, "Especifição: não foi possível ler o arquivo inserido: %s\n", filepath);
        return EXIT_FAILURE;
    }

    success_message(1);
    show_file_content(filepath, file_content, file_size);


    
    

    

}