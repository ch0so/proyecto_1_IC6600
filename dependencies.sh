#!/bin/bash
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
# Archivo bash que se encarga de la instalación de todas las dependencias necesarias para el uso adecuado de este proyecto,
# donde cada comando sudo corresponde a las distintas dependencias que se requieren. 
#
# Importante tener en cuenta:
# - Es necesario que se asegure que el compilador GCC y las herramientas de construcción básicas están instaladas
#   en un sistema de Fedora Workstation 40. Se pueden instalar las herramientas necesarias usando el siguiente comando:
#     sudo dnf install gcc make
# 
# - En el caso de no contar con Fedora WOrkstation 40, debe dirigirse a la página oficial e instalar el sistema operativo
#   que se encuentra en el siguiente enlace:
#     https://fedoraproject.org/es/workstation/download/
# 
# - En el link suminstrado en el anterior punto, puede encontrar la documentación oficial que le ayudará a realizar el proceso
#   de instalación adecuadamente.
# 
# - Para correr el archivo bash use el siguiente comando: bash dependencies.sh 
#   Esto facilita hará que se instalen todas las dependencias. 
#
# Referencias bibliográficas:
# 	-Wallen, J. (25 de julio, 2023). What is a Linux bash script and how do you create one? [¿Qué es un script de bash en Linux y cómo se crea uno?]. ZDNet. https://www.zdnet.com/article/what-is-a-linux-bash-script-and-how-do-you-create-one/
#*************************************************
#

sudo dnf install -y gcc make

sudo dnf install -y glibc-devel

sudo dnf install -y libpthread-stubs0-dev

sudo dnf install -y libcurl-devel

sudo dnf install -y tidy-devel

sudo dnf install -y pcre-devel

echo "Se han instalado todas las depedencias correctamente"

make


