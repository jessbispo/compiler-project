#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../../include/errorHandler.h"

ErrorTable* createErrorTable(unsigned int size) {
    ErrorTable* table = (ErrorTable*)malloc(sizeof(ErrorTable));
    if (!table) return NULL;

    table->errors = (Error**)calloc(size, sizeof(Error*));
    if (!table->errors) {
        free(table);
        return NULL;
    }

    table->size = size;
    table->count = 0;
    return table;
}

unsigned int hashError(const char* name, unsigned int tableSize) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash * 31 + *name) % tableSize;
        name++;
    }
    return hash;
}

int insertError(ErrorTable* table, const char* name, const char* description, ErrorType type) {
    if (!table || !name || !description) return -1;

    unsigned int index = hashError(name, table->size);
    Error* newError = (Error*)malloc(sizeof(Error));
    if (!newError) return -1;

    newError->name = strdup(name);
    newError->description = strdup(description);
    newError->timestamp = time(NULL);
    newError->type = type;
    newError->next = NULL;

    if (!newError->name || !newError->description) {
        free(newError->name);
        free(newError->description);
        free(newError);
        return -1;
    }

    // Handle collision using chaining
    if (table->errors[index]) {
        newError->next = table->errors[index];
    }
    table->errors[index] = newError;
    table->count++;

    return 0;
}

Error* findError(ErrorTable* table, const char* name) {
    if (!table || !name) return NULL;

    unsigned int index = hashError(name, table->size);
    Error* current = table->errors[index];

    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

int removeError(ErrorTable* table, const char* name) {
    if (!table || !name) return -1;

    unsigned int index = hashError(name, table->size);
    Error* current = table->errors[index];
    Error* prev = NULL;

    while (current) {
        if (strcmp(current->name, name) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                table->errors[index] = current->next;
            }

            free(current->name);
            free(current->description);
            free(current);
            table->count--;
            return 0;
        }
        prev = current;
        current = current->next;
    }

    return -1;
}

void destroyErrorTable(ErrorTable* table) {
    if (!table) return;

    for (unsigned int i = 0; i < table->size; i++) {
        Error* current = table->errors[i];
        while (current) {
            Error* next = current->next;
            free(current->name);
            free(current->description);
            free(current);
            current = next;
        }
    }

    free(table->errors);
    free(table);
}

void printErrorTable(const ErrorTable* table) {
    if (!table) return;

    printf("\n=== Error Table ===\n");
    printf("Total Errors: %u\n\n", table->count);

    for (unsigned int i = 0; i < table->size; i++) {
        Error* current = table->errors[i];
        if (current) {
            printf("Bucket %u:\n", i);
            while (current) {
                char* errorType;
                switch (current->type) {
                    case LEXICAL_ERROR:
                        errorType = "LEXICAL";
                        break;
                    case SYNTACTIC_ERROR:
                        errorType = "SYNTACTIC";
                        break;
                    case SEMANTIC_ERROR:
                        errorType = "SEMANTIC";
                        break;
                    case RUNTIME_ERROR:
                        errorType = "RUNTIME";
                        break;
                    default:
                        errorType = "UNKNOWN";
                }

                char timeStr[26];
                ctime_r(&current->timestamp, timeStr);
                timeStr[24] = '\0';  // Remove newline

                printf("  Name: %s\n", current->name);
                printf("  Description: %s\n", current->description);
                printf("  Type: %s\n", errorType);
                printf("  Timestamp: %s\n\n", timeStr);

                current = current->next;
            }
        }
    }
}

#ifdef BUILD_MAIN_STANDALONE
int main() {
    ErrorTable* table = createErrorTable(10);
    if (!table) {
        fprintf(stderr, "Failed to create error table.\n");
        return 1;
    }

    insertError(table, "Error1", "This is a lexical error.", LEXICAL_ERROR);
    insertError(table, "Error2", "This is a syntactic error.", SYNTACTIC_ERROR);
    insertError(table, "Error3", "This is a semantic error.", SEMANTIC_ERROR);

    printErrorTable(table);

    Error* found = findError(table, "Error2");
    if (found) {
        printf("Found Error: %s - %s\n", found->name, found->description);
    } else {
        printf("Error not found.\n");
    }

    removeError(table, "Error1");
    printErrorTable(table);

    destroyErrorTable(table);
    return 0;
}
#endif
