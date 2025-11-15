#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <time.h>

/**
 * @enum ErrorType
 * @brief Enumeration of possible error types in the compilation process
 */
typedef enum {
    LEXICAL_ERROR,
    SYNTACTIC_ERROR,
    SEMANTIC_ERROR,
    RUNTIME_ERROR
} ErrorType;

/**
 * @struct Error
 * @brief Structure to store information about a compilation error
 * 
 * @field name Name/identifier of the error
 * @field description Detailed description of the error
 * @field timestamp When the error occurred
 * @field type Type of the error (lexical, syntactic, semantic, runtime)
 * @field next Pointer to handle collision resolution in hash table
 */
typedef struct Error {
    char* name;
    char* description;
    time_t timestamp;
    ErrorType type;
    struct Error* next;
} Error;

/**
 * @struct ErrorTable
 * @brief Hash table structure to store compilation errors
 * 
 * @field size The size of the hash table
 * @field count Number of errors currently stored
 * @field errors Array of error entry pointers (collision lists)
 */
typedef struct {
    unsigned int size;
    unsigned int count;
    Error** errors;
} ErrorTable;

/**
 * @brief Creates a new error table with the specified size
 * 
 * @param size Initial size of the hash table
 * @return ErrorTable* Pointer to the newly created error table, NULL if allocation fails
 */
ErrorTable* createErrorTable(unsigned int size);

/**
 * @brief Computes the hash value for an error name
 * 
 * @param name The error name to hash
 * @param tableSize The size of the hash table
 * @return unsigned int The computed hash value
 */
unsigned int hashError(const char* name, unsigned int tableSize);

/**
 * @brief Inserts a new error into the error table
 * 
 * @param table The error table to insert into
 * @param name Name/identifier of the error
 * @param description Detailed description of the error
 * @param type Type of the error
 * @return int 0 on success, -1 on failure
 */
int insertError(ErrorTable* table, const char* name, const char* description, ErrorType type);

/**
 * @brief Retrieves an error from the table by its name
 * 
 * @param table The error table to search in
 * @param name The name of the error to find
 * @return Error* Pointer to the found error, NULL if not found
 */
Error* findError(ErrorTable* table, const char* name);

/**
 * @brief Removes an error from the table
 * 
 * @param table The error table to remove from
 * @param name The name of the error to remove
 * @return int 0 on success, -1 if error not found
 */
int removeError(ErrorTable* table, const char* name);

/**
 * @brief Cleans up and frees all memory used by the error table
 * 
 * @param table The error table to destroy
 */
void destroyErrorTable(ErrorTable* table);

/**
 * @brief Prints all errors in the table
 * 
 * @param table The error table to print
 */
void printErrorTable(const ErrorTable* table);

#endif /* ERROR_HANDLER_H */
