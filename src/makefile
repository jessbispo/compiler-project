CC = gcc
CFLAGS = -Wall -Wextra -std=c11

SRC = main.c ./header/beautiful-print.h ./impl/beautiful-print.c ./header/file-reader.h ./impl/file-reader.c
EXEC = main

# "compile" agora compila e executa diretamente
compile:
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)
	./$(EXEC) $(ARGS)

# Para limpar os arquivos compilados
clean:
	rm -f $(EXEC)
# Uso: make compile ARGS="arg1 arg2"
# Exemplo: make compile ARGS="input.txt output.txt"