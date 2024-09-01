# Nombre del ejecutable : project_1
#
# Versión : 1.0
#
# Fecha de creación: Agosto 29, 2024
#
# Descripción:
# Makefile para el primer proyecto de sistemas operativos.
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
# Referencias bibliográficas:
# 	-GNU. (s.f.). GNU Make Manual. Recuperado de  https://www.gnu.org/software/make/manual/make.html
#*************************************************

# Compilador a usar
CC = gcc

# Banderas del compilador
CFLAGS = -Wall -g

# Archivos de código fuente y objeto
SRCS = web_crawler.c mainThreads.c
OBJS = $(SRCS:.c=.o)

# Archivos de cabecera
HDRS = web_crawler.h

# Ejecutable

EXECUTABLE = project_1

# Objetivo por defecto
all: $(EXECUTABLE)

# Construir el ejecutable
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lcurl -ltidy

# Compilar archivos .c en archivos .o
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(OBJS)

.PHONY: all clean
