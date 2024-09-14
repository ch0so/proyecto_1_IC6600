# Nombre del ejecutable : project_1
#
# Versión : 1.0
#
# Fecha de creación: Setiembre 13, 2024
#
# Autores : 
#    - Ávila Ramírez Paublo Alexander (2022035584)
#    - Reyes Rodriguez Ricardo Andrés (2022101681)
#    - Zúñiga Campos Omar Jesús       (2022019053)
#
# Descripción:
# Este Makefile está diseñado para compilar una biblioteca estática y un programa ejecutable
# en un entorno Fedora. La biblioteca estática incluye implementaciones de mutex, semáforo binario,
# semáforo de conteo, barrera y candado de lectura/escritura. 
#
# Importante tener en cuenta:
# - Es necesario que se asegure que el compilador GCC y las herramientas de construcción básicas están instaladas
#   en un sistema de Fedora Workstation 39. Se pueden instalar las herramientas necesarias usando el siguiente comando:
#     sudo dnf install gcc make
# 
# - En el caso de no contar con Fedora WOrkstation 39, debe dirigirse a la página oficial e instalar el sistema operativo
#   que se encuentra en el siguiente enlace:
#     https://fedoraproject.org/es/workstation/download/
# 
# - En el link suminstrado en el anterior punto, puede encontrar la documentación oficial que le ayudará a realizar el proceso
#   de instalación adecuadamente.
#
# - El Makefile asume que los archivos fuente (.c) y de cabecera (.h) están en el mismo directorio
#   que el Makefile. Si se usan subdirectorios, se deben ajustar las rutas en las variables SRCS y HDRS.
# 
# - Antes de usar este Makefile, debe de correr el archivo dependencies.sh de la siguiente manera: bash dependencies.sh para asegurarse
#   de contar con todas las dependencias necesarias.
#
# Referencias bibliográficas:
# 	-GNU. (s.f.). GNU Make Manual. GNU. https://www.gnu.org/software/make/manual/make.html
#*************************************************

# Compilador a usar
CC = gcc

# Banderas del compilador
CFLAGS = -Wall -g

# Archivos de código fuente y objeto
SRCS = web_crawler.c huffman_serial.c huffman_fork.c huffman_threads.c main.c 
OBJS = web_crawler.o huffman_serial.o huffman_fork.o huffman_threads.o main.o

# Archivos de cabecera
HDRS = web_crawler.h huffman_fork.h huffman_serial.h huffman_threads.h

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
