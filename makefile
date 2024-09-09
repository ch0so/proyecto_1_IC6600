# Compilador a usar
CC = gcc

# Banderas del compilador
CFLAGS = -Wall -g

# Archivos de código fuente y objeto
SRCS = web_crawler.c main.c mainThreads.c
OBJS_MAIN = web_crawler.o main.o
OBJS_THREADS = web_crawler.o mainThreads.o

# Archivos de cabecera
HDRS = web_crawler.h

# Ejecutables
EXECUTABLE_MAIN = project_1
EXECUTABLE_THREADS = project_threads

# Objetivo por defecto: compilar ambos ejecutables
all: $(EXECUTABLE_MAIN) $(EXECUTABLE_THREADS)

# Construir el ejecutable principal (main.c)
$(EXECUTABLE_MAIN): $(OBJS_MAIN)
	$(CC) $(CFLAGS) -o $@ $(OBJS_MAIN) -lcurl -ltidy

# Construir el ejecutable con hilos (mainThreads.c)
$(EXECUTABLE_THREADS): $(OBJS_THREADS)
	$(CC) $(CFLAGS) -o $@ $(OBJS_THREADS) -lcurl -ltidy -lpthread

# Regla general para compilar archivos .c en archivos .o
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f *.o $(EXECUTABLE_MAIN) $(EXECUTABLE_THREADS)

.PHONY: all clean
