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

int insertError(ErrorTable* table, const char* name, const char* description, ErrorType type, int line, const char* filename) {
    if (!table || !name || !description) return -1;

    unsigned int index = hashError(name, table->size);
    Error* newError = (Error*)malloc(sizeof(Error));
    if (!newError) return -1;

    newError->name = strdup(name);
    newError->description = strdup(description);
    newError->timestamp = time(NULL);
    newError->type = type;
    newError->line = line;
    newError->filename = filename ? strdup(filename) : strdup("<unknown>");
    newError->snippet = NULL;
    newError->next = NULL;

    if (!newError->name || !newError->description || !newError->filename) {
        free(newError->name);
        free(newError->description);
        free(newError->filename);
        free(newError->snippet);
        free(newError);
        return -1;
    }

    /* Try to extract the exact line snippet from the source file, if available */
    if (filename && line > 0 && strcmp(filename, "<unknown>") != 0) {
        FILE *f = fopen(filename, "r");
        if (f) {
            char buf[2048];
            int cur = 1;
            while (fgets(buf, sizeof(buf), f)) {
                if (cur == line) {
                    /* Trim trailing newline */
                    size_t l = strlen(buf);
                    while (l > 0 && (buf[l-1] == '\n' || buf[l-1] == '\r')) { buf[--l] = '\0'; }
                    newError->snippet = strdup(buf[0] ? buf : "-");
                    break;
                }
                cur++;
            }
            fclose(f);
        }
    }
    if (!newError->snippet) newError->snippet = strdup("-");

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
            free(current->snippet);
            free(current->filename);
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
            free(current->snippet);
            free(current->filename);
            free(current);
            current = next;
        }
    }

    free(table->errors);
    free(table);
}

#ifdef BUILD_MAIN_STANDALONE
int main() {
    ErrorTable* table = createErrorTable(10);
    if (!table) {
        fprintf(stderr, "Failed to create error table.\n");
        return 1;
    }

    insertError(table, "Error1", "This is a lexical error.", LEXICAL_ERROR, 1, "<unknown>");
    insertError(table, "Error2", "This is a syntactic error.", SYNTACTIC_ERROR, 2, "<unknown>");
    insertError(table, "Error3", "This is a semantic error.", SEMANTIC_ERROR, 3, "<unknown>");

    printErrorTable(&table);

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
