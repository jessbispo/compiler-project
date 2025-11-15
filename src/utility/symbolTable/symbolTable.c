#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../include/symbolTable.h"

unsigned int hashFunction(const char *str) {
    unsigned int hash = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        hash = (hash * 31 + str[i]) % HASH_TABLE_SIZE;
    }
    return hash;
}

SymbolTable** initSymbolTable() {
    SymbolTable **hash_table = (SymbolTable**)calloc(HASH_TABLE_SIZE, sizeof(SymbolTable*));
    if (!hash_table) {
        fprintf(stderr, "Memory allocation failed for hash table\n");
        return NULL;
    }
    return hash_table;
}

SymbolTable* createSymbol(const char *name, const char *category, const char *type, const char *scope,
                           const char *address, int size, const char *parameters,
                           const char *literal_value, const char *declared_in,
                           const char *visibility, int line) {
    SymbolTable *new_symbol = (SymbolTable *)malloc(sizeof(SymbolTable));
    if (!new_symbol) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    new_symbol->name = name ? strdup(name) : strdup("-");
    new_symbol->category = category ? strdup(category) : strdup("-");
    new_symbol->type = type ? strdup(type) : strdup("-");
    new_symbol->scope = scope ? strdup(scope) : strdup("-");
    new_symbol->address = address ? strdup(address) : strdup("-");
    new_symbol->size = size;
    new_symbol->parameters = parameters ? strdup(parameters) : strdup("-");
    new_symbol->literal_value = literal_value ? strdup(literal_value) : strdup("-");
    new_symbol->declared_in = declared_in ? strdup(declared_in) : strdup("-");
    new_symbol->visibility = visibility ? strdup(visibility) : strdup("-");
    new_symbol->line = line;
    new_symbol->next = NULL;
    return new_symbol;
}

int insertSymbol(SymbolTable **hash_table, SymbolTable *symbol) {
    if (!hash_table || !symbol) return 0;
    
    unsigned int index = hashFunction(symbol->name);
    
    // Se a posição estiver vazia
    if (hash_table[index] == NULL) {
        hash_table[index] = symbol;
        return 1;
    }
    
    // Tratamento de colisão por encadeamento
    SymbolTable *current = hash_table[index];
    while (current->next != NULL) {
        if (strcmp(current->name, symbol->name) == 0) {
            // Símbolo já existe
            return 0;
        }
        if (current->next == NULL) break;
        current = current->next;
    }
    
    // Adiciona no final da lista
    current->next = symbol;
    return 1;
}

SymbolTable* searchSymbol(SymbolTable **hash_table, const char *name) {
    if (!hash_table || !name) return NULL;
    
    unsigned int index = hashFunction(name);
    
    // Procura na lista encadeada da posição hash
    SymbolTable *current = hash_table[index];
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

#ifdef BUILD_MAIN_STANDALONE
int main() {
    printf("[ SYMBOL TABLE TESTING MODE ]\n");

    SymbolTable **symbol_table = initSymbolTable();
    if (!symbol_table) return 1;

    SymbolTable *sym1 = createSymbol("varA", "variable", "int", "global", "-", 0, "-", "-", "-", "-", 1);
    SymbolTable *sym2 = createSymbol("funcB", "function", "void", "global", "-", 0, "-", "-", "-", "-", 5);

    insertSymbol(symbol_table, sym1);
    insertSymbol(symbol_table, sym2);

    printf("- symbol table:\n");
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        SymbolTable *current = symbol_table[i];
        while (current != NULL) {
            printf("Symbol: %s, Type: %s, Scope: %s, Line: %d\n",
                   current->name, current->type, current->scope, current->line);
            current = current->next;
        }
    }

    printf("- searching for 'varA':\n");
    SymbolTable *found = searchSymbol(symbol_table, "varA");
    if (found) {
        printf("- found symbol: %s of type %s\n", found->name, found->type);
    } else {
        printf("- symbol not found\n");
    }

    // Free allocated memory (not shown here for brevity)

    return 0;
}
#endif