#include <stdio.h>
#include <stdlib.h>
#include "../header/file-reader.h"

int read_file(char *filename, char **buffer, size_t *size) {
    FILE *file = fopen(filename, "r");
    
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = (char *)malloc(*size + 1);
    if (!*buffer) {
        fclose(file);
        return -1;
    }

    fread(*buffer, 1, *size, file);
    (*buffer)[*size] = '\0';

    fclose(file);
    return 0;

}