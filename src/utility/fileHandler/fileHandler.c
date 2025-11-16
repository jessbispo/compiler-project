/** @brief Implementação das funções do módulo FileHandler.
 *  @details A documentação de cada função está presente no arquivo de cabeçalho correspondente.
 *  @headerfile fileHandler.h
 *  @authors Jessica Bispo (10410798), Vitor Alves Pereira (10410862)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../../../include/fileHandler.h"

int closeFile(FILE *file) {
    if (file == NULL) {
        return 1;
    }
    if (fclose(file) != 0) {
        return 1;
    }
    return 0;
}

int validateFilePath(const char *filepath) {
    return (filepath == NULL || filepath[0] == '\0') ? 1 : 0;
}

int openFile(FILE **file, const char *filepath, char *mode) {
    if (validateFilePath(filepath)) return 1;
    *file = fopen(filepath, mode);
    return (*file != NULL) ? 0 : 1;
}

int readFileContent(const char *filepath, char **contentBuffer) {
    FILE *file;
    long fileSize;
    size_t bytesRead = 0;
    if (openFile(&file, filepath, "r") != 0) {
        return -1;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        closeFile(file);
        return -1;
    }

    fileSize = ftell(file);
    if (fileSize < 0) {
        closeFile(file);
        return -1;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        closeFile(file);
        return -1;
    }

    *contentBuffer = (char *)malloc((size_t)fileSize + 1);
    if (*contentBuffer == NULL) {
        closeFile(file);
        return -1;
    }

    bytesRead = fread(*contentBuffer, 1, (size_t)fileSize, file);
    (*contentBuffer)[bytesRead] = '\0';

    closeFile(file);
    return (int)bytesRead;
}

int createFile(const char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        return 1;
    }
    closeFile(file);
    return 0;
}

int writeOnFile(const char *filepath, const char *content) {
    FILE *file;
    if (openFile(&file, filepath, "w") != 0) {
        return 1;
    }

    if (fputs(content, file) == EOF) {
        closeFile(file);
        return 1;
    }

    closeFile(file);
    return 0;
}

#ifdef BUILD_MAIN_STANDALONE
int main() {
    FILE *file;
    char filepath[256];
    char mode[4];

    char *contentBuffer = NULL;
    int bytesRead;

    printf("[ FILE HANDLER TESTING MODE ]\n");

    printf("- enter the filepath: ");
    if (scanf("%255s", filepath) != 1) return 1;

    printf("- enter the mode (r, w, a, rb, wb, ab): ");
    if (scanf("%3s", mode) != 1) return 1;

    if (openFile(&file, filepath, mode) != 0) {
        printf("- error: could not open file '%s' with mode '%s'\n", filepath, mode);
        return 1;
    }

    printf("- file '%s' opened successfully with mode '%s'\n", filepath, mode);

    closeFile(file);

    printf("- file content:\n");
    bytesRead = readFileContent(filepath, &contentBuffer);

    if (bytesRead < 0) {
        printf("- error: could not read file '%s'\n", filepath);
        return 1;
    } else {
        printf("%s\n", contentBuffer);
        free(contentBuffer);
    }

    return 0;
}
#endif