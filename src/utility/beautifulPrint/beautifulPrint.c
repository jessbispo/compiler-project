#include <stdio.h>
#include "../../../include/beautifulPrint.h"
#include "../../../include/symbolTable.h"
#include "../../../include/syntacticAnalysis.h"

void printTitle() {
    printf(BOLD_CYAN
        "███╗   ███╗██╗███╗   ██╗██╗██████╗  █████╗ ███████╗ ██████╗ █████╗ ██╗          ██████╗ ██████╗ ███╗   ███╗██████╗ ██╗██╗     ███████╗██████╗     ██████╗ ██████╗  ██████╗      ██╗███████╗ ██████╗████████╗\n"
        "████╗ ████║██║████╗  ██║██║██╔══██╗██╔══██╗██╔════╝██╔════╝██╔══██╗██║         ██╔════╝██╔═══██╗████╗ ████║██╔══██╗██║██║     ██╔════╝██╔══██╗    ██╔══██╗██╔══██╗██╔═══██╗     ██║██╔════╝██╔════╝╚══██╔══╝\n"
        "██╔████╔██║██║██╔██╗ ██║██║██████╔╝███████║███████╗██║     ███████║██║         ██║     ██║   ██║██╔████╔██║██████╔╝██║██║     █████╗  ██████╔╝    ██████╔╝██████╔╝██║   ██║     ██║█████╗  ██║        ██║   \n"
        "██║╚██╔╝██║██║██║╚██╗██║██║██╔═══╝ ██╔══██║╚════██║██║     ██╔══██║██║         ██║     ██║   ██║██║╚██╔╝██║██╔═══╝ ██║██║     ██╔══╝  ██╔══██╗    ██╔═══╝ ██╔══██╗██║   ██║██   ██║██╔══╝  ██║        ██║   \n"
        "██║ ╚═╝ ██║██║██║ ╚████║██║██║     ██║  ██║███████║╚██████╗██║  ██║███████╗    ╚██████╗╚██████╔╝██║ ╚═╝ ██║██║     ██║███████╗███████╗██║  ██║    ██║     ██║  ██║╚██████╔╝╚█████╔╝███████╗╚██████╗   ██║   \n"
        "╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝╚═╝     ╚═╝  ╚═╝╚══════╝ ╚═════╝╚═╝  ╚═╝╚══════╝     ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝    ╚═╝     ╚═╝  ╚═╝ ╚═════╝  ╚════╝ ╚══════╝ ╚═════╝   ╚═╝ \n"
        RESET
        "\n"
        WHITE"      feito por "BOLD_GREEN"Vitor Pereira"RESET" e "BOLD_MAGENTA"Jessica Bispo"RESET"\n"
        BOLD_YELLOW"                Compiladores 2025/2\n"RESET
        "\n"
        BOLD_BLUE"[+] "CYAN"Análise Léxica\n"
        BOLD_BLUE"[+] "CYAN"Análise Sintática\n"
        BOLD_BLUE"[+] "CYAN"Análise Semântica\n"
        BOLD_BLUE"[+] "CYAN"Geração de Código Intermediário\n"RESET
    );

}

void printErrorTable(ErrorTable **error_table) {
    printf("\n%-6s | %-20s | %-55s | %-30s | %-5s\n", "TYPE", "MESSAGE", "SNIPPET", "FILENAME", "LINE");
    printf("-------------------------------------------------------------------------------------------------------------------------------\n");

    if (!error_table || !(*error_table)) {
        printf("Tabela de erros vazia.\n");
        return;
    }

    ErrorTable *table = *error_table;
    
    if (table->count == 0) {
        printf("Tabela de erros vazia.\n");
        return;
    }

    int found = 0;
    for (unsigned int i = 0; i < table->size; ++i) {
        Error *current = table->errors[i];
        while (current) {
            printf("%-6s | %-20s | %-55s | %-30s | %-5d\n",
                   "ERROR",
                   current->description && current->description[0] ? current->description : "-",
                   current->snippet && current->snippet[0] ? current->snippet : "-",
                   current->filename && current->filename[0] ? current->filename : "-",
                   current->line);
            found = 1;
            current = current->next;
        }
    }

    if (!found) {
        printf("Tabela de erros vazia.\n");
    }
}

void printSymbolTable(SymbolTable **hash_table) {
        printf("\n%-12s | %-12s | %-10s | %-10s | %-8s | %-6s | %-12s | %-14s | %-12s | %-10s | %-5s\n",
            "SYMBOL", "CATEGORY", "TYPE", "SCOPE", "ADDRESS", "SIZE", "PARAMETERS", "LITERAL_VALUE", "DECLARED_IN", "VISIBILITY", "LINE");
        printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");

    if (!hash_table) {
        printf("Tabela de símbolos vazia.\n");
        return;
    }

    int found = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        SymbolTable *current = hash_table[i];
        while (current) {
            char size_buf[32];
            const char *size_str = "-";
            if (current->size != 0) {
                snprintf(size_buf, sizeof(size_buf), "%d", current->size);
                size_str = size_buf;
            }
            printf("%-12s | %-12s | %-10s | %-10s | %-8s | %-6s | %-12s | %-14s | %-12s | %-10s | %-5d\n",
                   current->name && current->name[0] ? current->name : "-",
                   current->category && current->category[0] ? current->category : "-",
                   current->type && current->type[0] ? current->type : "-",
                   current->scope && current->scope[0] ? current->scope : "-",
                   current->address && current->address[0] ? current->address : "-",
                   size_str,
                   current->parameters && current->parameters[0] ? current->parameters : "-",
                   current->literal_value && current->literal_value[0] ? current->literal_value : "-",
                   current->declared_in && current->declared_in[0] ? current->declared_in : "-",
                   current->visibility && current->visibility[0] ? current->visibility : "-",
                   current->line);
            found = 1;
            current = current->next;
        }
    }

    if (!found) {
        printf("Tabela de símbolos vazia.\n");
    }
}

void printSubtitle(char *message) {
    printf(BOLD_YELLOW"%s\n"RESET, message);
}

void printMessage(char *message, int messageType) {
    switch (messageType) {
        case 1:
            printf(BOLD_GREEN"[INFO]: "RESET"%s\n", message);
            break;
        case 2:
            printf(BOLD_YELLOW"[WARNING]: "RESET"%s\n", message);
            break;
        case 3:
            printf(BOLD_RED"[ERROR]: "RESET"%s\n", message);
            break;
        default:
            printf("[UNKNOWN]: %s\n", message);
            break;
    }
}

void printDivisor() {
    printf(GREY"\n+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n\n"RESET);
}

void receiveEntry(char *message, int entryType) {
    switch (entryType) {
        case 1:
            printf(BOLD_GREEN"[INPUT]: "RESET"%s", message);
            break;
    }
}

#ifdef BUILD_MAIN_STANDALONE
int main() {
    printTitle();
    return 0;
}
#endif
