#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/fileHandler.h"
#include "../include/beautifulPrint.h"
#include "../include/symbolTable.h"
#include "../include/errorHandler.h"
#include "../include/lexicalAnalysis.h"

int main(int argc, char *argv[]) {
    FILE *file;
    SymbolTable **symbolTable;
    ErrorTable *errorTable;
    char *contentBuffer = NULL;

    printTitle();
    printDivisor();

    if (argc == 1) {
        char filepath[256];
        
        printMessage("nenhum arquivo foi fornecido como entrada.", 2);
        printMessage("realizando a leitura do arquivo manualmente...", 2);

        receiveEntry("insira o caminho do arquivo a ser lido: ", 1);
        scanf("%255s", filepath);

        while(validateFilePath(filepath)) {
            printMessage("não foi possível identificar o arquivo no caminho fornecido. Tente novamente...", 3);
            receiveEntry("insira o caminho do arquivo a ser lido: ", 1);
            scanf("%255s", filepath);
        }

        if (openFile(&file, filepath, "r")) {
            printMessage("falha ao abrir o arquivo.", 3);
            return EXIT_FAILURE;
        } else {
            printMessage("arquivo aberto com sucesso!", 1);
            readFileContent(filepath, &contentBuffer);
            if (!contentBuffer || strlen(contentBuffer) == 0) {
                printMessage("arquivo vazio ou erro ao ler conteúdo.", 3);
                closeFile(file);
                return EXIT_FAILURE;
            }
            closeFile(file);
        }

    } else {
        if (openFile(&file, argv[1], "r")) {
            printMessage("falha ao abrir o arquivo.", 3);
            return EXIT_FAILURE;
        } else {
            printMessage("arquivo aberto com sucesso!", 1);
            readFileContent(argv[1], &contentBuffer);
            if (!contentBuffer || strlen(contentBuffer) == 0) {
                printMessage("arquivo vazio ou erro ao ler conteúdo.", 3);
                closeFile(file);
                return EXIT_FAILURE;
            }
            closeFile(file);
        }
    }

    printDivisor();
    printSubtitle("Análise Léxica");

    symbolTable = initSymbolTable();
    errorTable = createErrorTable(256);

    lexicalAnalysis(contentBuffer, symbolTable, errorTable);

    printSymbolTable(symbolTable);

    return 0;
}