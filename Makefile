# ==============================================================================
# MiniPascal Compiler - Makefile
# ==============================================================================
# Project: Compilador MiniPascal
# Description: Sistema de compilação para o compilador MiniPascal
# ==============================================================================

.PHONY: help all compiler clean clean-all clean-build clean-bin rebuild \
        run test-compiler install uninstall info

# ==============================================================================
# Configuração do Compilador
# ==============================================================================

CC          := gcc
CFLAGS      := -Wall -Wextra -Wno-unused-result -g -O2 -I include
LDFLAGS     := 

# Suporte para debug adicional
DEBUG       ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -O0 -g3
endif

# ==============================================================================
# Diretórios
# ==============================================================================

SRC_DIR     := src
BUILD_DIR   := build
BIN_DIR     := bin
INCLUDE_DIR := include

# ==============================================================================
# Variáveis de Saída
# ==============================================================================

COMPILER    := $(BIN_DIR)/compiler

# ==============================================================================
# Arquivos Fonte - Frontend
# ==============================================================================

FRONTEND_SRC := \
	$(SRC_DIR)/frontend/lexicalAnalysis.c \
	$(SRC_DIR)/frontend/semanticAnalysis.c \
	$(SRC_DIR)/frontend/syntacticAnalysis.c \
	$(SRC_DIR)/frontend/intermediateCodeGenerator.c

# ==============================================================================
# Arquivos Fonte - Utility (AFN)
# ==============================================================================

AFN_SRC := \
	$(SRC_DIR)/utility/afn/comment-afn.c \
	$(SRC_DIR)/utility/afn/datatype-afn.c \
	$(SRC_DIR)/utility/afn/delimiter-afn.c \
	$(SRC_DIR)/utility/afn/identifier-afn.c \
	$(SRC_DIR)/utility/afn/keyword-afn.c \
	$(SRC_DIR)/utility/afn/operator-afn.c

# ==============================================================================
# Arquivos Fonte - Utility (Outros)
# ==============================================================================

UTILITY_SRC := \
	$(SRC_DIR)/utility/errorHandler/errorHandler.c \
	$(SRC_DIR)/utility/fileHandler/fileHandler.c \
	$(SRC_DIR)/utility/symbolTable/symbolTable.c \
	$(SRC_DIR)/utility/beautifulPrint/beautifulPrint.c

# ==============================================================================
# Arquivos Fonte - Principal
# ==============================================================================

MAIN_SRC    := $(SRC_DIR)/main.c

# ==============================================================================
# Compilação de Todas as Fontes
# ==============================================================================

ALL_SRC     := $(MAIN_SRC) $(FRONTEND_SRC) $(AFN_SRC) $(UTILITY_SRC)
ALL_OBJ     := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(ALL_SRC))

# ==============================================================================
# Regras Padrão (Alvos)
# ==============================================================================

all: compiler
	@echo "✓ Compilação concluída com sucesso!"

compiler: $(COMPILER)
	@echo "✓ Compilador criado: $(COMPILER)"

# ==============================================================================
# Compilação de Objetos
# ==============================================================================

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "Compilando: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# ==============================================================================
# Linking - Executável Principal
# ==============================================================================

$(COMPILER): $(ALL_OBJ) | $(BIN_DIR)
	@echo "Linkando: $(COMPILER)"
	@$(CC) $(CFLAGS) $(LDFLAGS) $(ALL_OBJ) -o $@
	@echo "✓ Executável criado: $@"

# ==============================================================================
# Criação de Diretórios
# ==============================================================================

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# ==============================================================================
# Alvos de Limpeza
# ==============================================================================

clean: clean-build clean-bin
	@echo "✓ Limpeza completa realizada"

clean-build:
	@echo "Removendo: $(BUILD_DIR)"
	@rm -rf $(BUILD_DIR)

clean-bin:
	@echo "Removendo: $(BIN_DIR)"
	@rm -rf $(BIN_DIR)

clean-all: clean
	@echo "✓ Limpeza total concluída"

# ==============================================================================
# Reconstrução
# ==============================================================================

rebuild: clean compiler
	@echo "✓ Reconstrução concluída"

# ==============================================================================
# Execução
# ==============================================================================

run: compiler
	@echo "Executando compilador..."
	@./$(COMPILER)

# ==============================================================================
# Informações
# ==============================================================================

info:
	@echo "╔════════════════════════════════════════════════════════╗"
	@echo "║          MiniPascal Compiler - Build Info             ║"
	@echo "╚════════════════════════════════════════════════════════╝"
	@echo "Compilador:      $(CC)"
	@echo "Flags:           $(CFLAGS)"
	@echo "Diretório Build: $(BUILD_DIR)"
	@echo "Diretório Bin:   $(BIN_DIR)"
	@echo "Executável:      $(COMPILER)"
	@echo ""
	@echo "Arquivos Fonte Frontend:"
	@echo "  - $(FRONTEND_SRC)"
	@echo ""
	@echo "Arquivos Fonte Utility:"
	@echo "  - AFN: $(AFN_SRC)"
	@echo "  - Outros: $(UTILITY_SRC)"
	@echo ""

help:
	@echo "╔════════════════════════════════════════════════════════╗"
	@echo "║    MiniPascal Compiler - Make Targets                 ║"
	@echo "╚════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Alvos Principais:"
	@echo "  make all              - Compila o compilador (padrão)"
	@echo "  make compiler         - Compila apenas o executável"
	@echo "  make rebuild          - Limpa e recompila tudo"
	@echo ""
	@echo "Limpeza:"
	@echo "  make clean            - Remove build/ e bin/"
	@echo "  make clean-build      - Remove apenas build/"
	@echo "  make clean-bin        - Remove apenas bin/"
	@echo "  make clean-all        - Limpeza completa"
	@echo ""
	@echo "Execução:"
	@echo "  make run              - Compila e executa o compilador"
	@echo ""
	@echo "Informações:"
	@echo "  make info             - Exibe configuração de build"
	@echo "  make help             - Exibe esta mensagem"
	@echo ""
	@echo "Variáveis de Ambiente:"
	@echo "  DEBUG=1               - Ativa modo debug durante compilação"
	@echo "                         (make DEBUG=1)"
	@echo ""

# ==============================================================================
# Depuração
# ==============================================================================

.PHONY: debug
debug:
	@echo "Alvos disponíveis:"
	@echo "  ALL_SRC: $(ALL_SRC)"
	@echo "  ALL_OBJ: $(ALL_OBJ)"
	@echo ""
	@echo "CFLAGS: $(CFLAGS)"
	@echo "CC: $(CC)"
