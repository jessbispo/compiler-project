CC=gcc
CFLAGS=-Wall -Wno-unused-result -g -Og -I include
BIN_DIR=bin
BUILD_DIR=build
INCLUDE_DIR=include

# Main compiler target
COMPILER=$(BIN_DIR)/compiler
COMPILER_SRC=src/main.c
COMPILER_OBJ=$(BUILD_DIR)/main.o

# Common source files
COMMON_SRC=\
	frontend/lexicalAnalysis.c \
	frontend/semanticAnalysis.c \
	frontend/syntacticAnalysis.c \
	frontend/intermediateCodeGenerator.c \
	utility/afn/impl/comment-afn.c \
	utility/afn/impl/datatype-afn.c \
	utility/afn/impl/delimiter-afn.c \
	utility/afn/impl/identifier-afn.c \
	utility/afn/impl/keyword-afn.c \
	utility/afn/impl/operator-afn.c \
	utility/errorHandler/errorHandler.c \
	utility/fileHandler/fileHandler.c \
	utility/symbolTable/symbolTable.c \
	utility/beautifulPrint/beautifulPrint.c

COMMON_OBJ=$(COMMON_SRC:%.c=$(BUILD_DIR)/%.o)

# Standalone test executables
TEST_BEAUTIFUL_PRINT=$(BIN_DIR)/beautifulPrint
TEST_ERROR_HANDLER=$(BIN_DIR)/runErrorHandler
TEST_FILE_HANDLER=$(BIN_DIR)/runFileHandler
TEST_SYMBOL_TABLE=$(BIN_DIR)/runSymbolTable

# Headers
HEADERS=\
	include/lexicalAnalysis.h \
	include/syntacticAnalysis.h \
	src/utility/afn/header/comment-afn.h \
	src/utility/afn/header/datatype-afn.h \
	src/utility/afn/header/delimiter-afn.h \
	src/utility/afn/header/identifier-afn.h \
	src/utility/afn/header/keyword-afn.h \
	src/utility/afn/header/operator-afn.h

# Default target
all: $(COMPILER)

# Create directories
$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile main compiler
$(COMPILER): $(COMPILER_OBJ) $(COMMON_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(COMPILER_OBJ): $(COMPILER_SRC) | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Pattern rule for common object files
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Test targets
test-beautiful-print: $(TEST_BEAUTIFUL_PRINT)
test-error-handler: $(TEST_ERROR_HANDLER)
test-file-handler: $(TEST_FILE_HANDLER)
test-symbol-table: $(TEST_SYMBOL_TABLE)

all-tests: $(TEST_BEAUTIFUL_PRINT) $(TEST_ERROR_HANDLER) $(TEST_FILE_HANDLER) $(TEST_SYMBOL_TABLE)

# Clean targets
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

clean-build:
	rm -rf $(BUILD_DIR)

clean-bin:
	rm -rf $(BIN_DIR)

.PHONY: all clean clean-build clean-bin test-beautiful-print test-error-handler test-file-handler test-symbol-table all-tests
