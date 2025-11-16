#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/fileHandler.h"
#include "../include/beautifulPrint.h"
#include "../include/symbolTable.h"
#include "../include/errorHandler.h"
#include "../include/lexicalAnalysis.h"
#include "../include/syntacticAnalysis.h"
#include "../include/semanticAnalysis.h"
#include "../include/intermediateCodeGenerator.h"

int main(int argc, char *argv[]) {
    FILE *file;
    SymbolTable **symbolTable;
    ErrorTable *errorTable;
    char *contentBuffer = NULL;
    char filepath[256] = {0};

    printTitle();
    printDivisor();

    if (argc == 1) {
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
        strncpy(filepath, argv[1], sizeof(filepath)-1);
        filepath[sizeof(filepath)-1] = '\0';
        
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
    }

    printDivisor();
    printSubtitle("Análise Léxica");

    symbolTable = initSymbolTable();
    if (!symbolTable) {
        printMessage("falha ao inicializar tabela de símbolos.", 3);
        free(contentBuffer);
        return EXIT_FAILURE;
    }

    errorTable = createErrorTable(256);
    if (!errorTable) {
        printMessage("falha ao criar tabela de erros.", 3);
        free(contentBuffer);
        return EXIT_FAILURE;
    }

    Token *tokens = lexicalAnalysis(contentBuffer, filepath, symbolTable, errorTable);
    if (!tokens) {
        printMessage("falha na análise léxica.", 3);
        free(contentBuffer);
        return EXIT_FAILURE;
    }

    printSymbolTable(symbolTable);
    
    if (errorTable && errorTable->count > 0) {
        printf("\n");
        printMessage("erros encontrados na análise léxica:", 3);
        printErrorTable(&errorTable);
        free(contentBuffer);
        if (tokens) free(tokens);
        if (symbolTable) destroySymbolTable(symbolTable);
        if (errorTable) destroyErrorTable(errorTable);
        return EXIT_FAILURE;
    }

    free(contentBuffer);

    printDivisor();
    printSubtitle("Análise Sintática");

    Parser parser = createParser(tokens, symbolTable, errorTable);
    int syntaxResult = syntacticAnalysis(&parser);

    if (syntaxResult == 0 && !(errorTable && errorTable->count > 0)) {
        printMessage("análise sintática concluída com sucesso!", 1);
    } else {
        printMessage("falha na análise sintática.", 3);
        if (errorTable && errorTable->count > 0) {
            printErrorTable(&errorTable);
        }
        if (parser.root) freeAST(parser.root);
        if (tokens) free(tokens);
        if (errorTable) destroyErrorTable(errorTable);
        if (symbolTable) destroySymbolTable(symbolTable);
        return EXIT_FAILURE;
    }

    if (!parser.root) {
        printMessage("falha ao criar árvore sintática.", 3);
        free(tokens);
        return EXIT_FAILURE;
    }

    printDivisor();
    printSubtitle("Análise Semântica");

    // Limpar a tabela de símbolos antes da análise semântica
    clearSymbolTable(symbolTable);
    
    int semanticResult = semanticAnalysis(parser.root, symbolTable, errorTable, filepath);
    if (semanticResult == 0 && !(errorTable && errorTable->count > 0)) {
        printMessage("análise semântica concluída com sucesso!", 1);
    } else {
        printMessage("falha na análise semântica.", 3);
        if (errorTable && errorTable->count > 0) {
            printErrorTable(&errorTable);
        }
        if (parser.root) freeAST(parser.root);
        if (tokens) free(tokens);
        if (errorTable) destroyErrorTable(errorTable);
        if (symbolTable) destroySymbolTable(symbolTable);
        return EXIT_FAILURE;
    }

    printDivisor();
    printSubtitle("Geração de Código Intermediário");

    IntermediateCode *intermediateCode = generateIntermediateCode(parser.root, symbolTable, errorTable);
    
    if (!intermediateCode) {
        printMessage("falha ao gerar código intermediário.", 3);
        if (parser.root) freeAST(parser.root);
        if (tokens) free(tokens);
        if (errorTable) destroyErrorTable(errorTable);
        if (symbolTable) destroySymbolTable(symbolTable);
        return EXIT_FAILURE;
    }

    printMessage("código intermediário gerado com sucesso!", 1);
    
    /* Print intermediate code to terminal */
    printIntermediateCode(intermediateCode);

    /* Generate output filename based on input filename */
    char outputFilepath[256] = {0};
    if (strlen(filepath) > 0) {
        /* Remove .txt/.pascal extension and add .tac */
        char *dot = strrchr(filepath, '.');
        if (dot) {
            int nameLen = dot - filepath;
            strncpy(outputFilepath, filepath, nameLen);
            strcat(outputFilepath, ".tac");
        } else {
            strcpy(outputFilepath, filepath);
            strcat(outputFilepath, ".tac");
        }
    } else {
        strcpy(outputFilepath, "output.tac");
    }

    /* Write intermediate code to file */
    if (writeIntermediateCodeToFile(intermediateCode, outputFilepath) == 0) {
        printMessage("código intermediário salvo em: ", 1);
        printf("  %s\n\n", outputFilepath);
    } else {
        printMessage("aviso: não foi possível salvar o arquivo de código intermediário.", 2);
    }

    printDivisor();

    // Liberar memória alocada
    if (intermediateCode) {
        freeIntermediateCode(intermediateCode);
    }
    if (parser.root) {
        freeAST(parser.root);
    }
    if (tokens) {
        free(tokens);
    }
    if (errorTable) {
        destroyErrorTable(errorTable);
    }
    if (symbolTable) {
        destroySymbolTable(symbolTable);
    }

    return 0;
}