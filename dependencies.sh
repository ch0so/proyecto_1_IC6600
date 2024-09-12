#!/bin/bash

sudo dnf install -y gcc make

sudo dnf install -y glibc-devel

sudo dnf install -y libpthread-stubs0-dev

sudo dnf install -y libcurl-devel

sudo dnf install -y tidy-devel

sudo dnf install -y pcre-devel

echo "Se han instalado todas las depedencias correctamente"
