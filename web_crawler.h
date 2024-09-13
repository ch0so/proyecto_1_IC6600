/*
Código creado por:
    Ávila Ramírez Paublo Alexander
    pavila@estudiantec.cr
    2022035584
    
    Reyes Rodríguez	Ricardo Andrés
    rireyes@estudiantec.cr
    2022101681

	Zúñiga Campos Omar Jesús
    omzuniga@estudiantec.cr
    2022019053

Aclaración:
- Se coloca la referencia al proyecto gutenberg debido a que de este web crawler fue hecho para usarse en dicho sitio. Además que los libros son extraídos de ahí.

Referencias bibliográficas:
    -Gutenberg, P. (s.f.). Proyecto Gutenberg. Proyecto Gutenberg. https://www.gutenberg.org/
    -TidyLib. (1 de marzo, 2017). tidybuffio.h [Documentación de la API]. TidyLib. https://api.html-tidy.org/tidy/tidylib_api_5.4.0/tidybuffio_8h.html
    -ZenRows. (2024, 1 junio). Web scraping with C in 2024 [Raspado WEB con C en 2024]. ZenRows. https://www.zenrows.com/blog/web-scraping-c#get-the-html
*/

#ifndef WEB_CRAWLER_H
#define WEB_CRAWLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <regex.h>
#include <tidy.h>
#include <tidybuffio.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Estructura que almacena datos en memoria junto con su tamaño
typedef struct {
    char *data; // Puntero a los datos en memoria
    size_t size; // Tamaño de los datos en bytes
} memory_data;

// Función que escribe datos en memoria durante una transferencia de CURL
size_t write_memory_callback(void *contents, size_t size, size_t number_of_members, void *user_pointer);

// Función que extrae enlaces de una cadena HTML y los guarda en un archivo
void extract_links_from_html(const char *html);

// Función que encuentra un enlace en una cadena HTML que coincida con un patrón específico
char *find_link(const char *html);

// Función que extrae el título y el autor de un archivo de texto y los guarda en las variables proporcionadas
void extract_title_and_author(const char *filename, char **title, char **author);

// Función que genera un nombre de archivo único basado en un nombre base
char *generate_unique_filename(const char *base_path);

// Función que procesa un enlace utilizando CURL para descargar datos y luego los procesa
void process_link(CURL *curl, const char *url);

// Función que elimina un directorio y su contenido de forma recursiva
int remove_directory(const char *dir_path);

// Función que descarga archivos de texto desde una URL base y los procesa
void download_text_files(const char* base_url);


#endif // WEB_CRAWLER_H