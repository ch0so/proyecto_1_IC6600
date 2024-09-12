# Compilador a usar
CC = gcc

# Banderas del compilador
CFLAGS = -Wall -g

# Archivos de código fuente y objeto
SRCS = web_crawler.c huffman_fork.c main.c 
OBJS = web_crawler.o huffman_fork.o main.o

# Archivos de cabecera
HDRS = web_crawler.h huffman_fork.h

# Ejecutables
EXECUTABLE_MAIN = project_1

# Objetivo por defecto: compilar ambos ejecutables
all: $(EXECUTABLE_MAIN)

$(EXECUTABLE_MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lcurl -ltidy

# Regla general para compilar archivos .c en archivos .o
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f *.o $(EXECUTABLE_MAIN)

.PHONY: all clean
