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

#include "web_crawler.h"

size_t write_memory_callback(void *contents, size_t size, size_t number_of_members, void *user_pointer) {
    /*
        Función que escribe datos en memoria durante una transferencia de CURL

        Entradas:
        - contents: Puntero a los datos a escribir
        - size: Tamaño de cada elemento de datos
        - number_of_members: Número de elementos de datos
        - user_pointer: Puntero a una estructura de datos en memoria que se debe actualizar

        Salidas:
        - real_size: Número de bytes realmente escritos

        Restricciones:
        - La variable contents no debe ser NULL
        - La variable user_pointer debe apuntar a una estructura de tipo memory_data válida
    */
    size_t real_size = size * number_of_members;
    memory_data *memory = (memory_data *)user_pointer;

    char *new_pointer = realloc(memory->data, memory->size + real_size + 1);
    if (new_pointer == NULL) {
        printf("No hay suficiente memoria (realloc devolvió NULL)\n");
        return 0;
    }

    memory->data = new_pointer;
    memcpy(&(memory->data[memory->size]), contents, real_size);
    memory->size += real_size;
    memory->data[memory->size] = '\0';

    return real_size;
}

void extract_links_from_html(const char *html) {
    /*
        Función que extrae enlaces de una cadena HTML y los guarda en un archivo

        Entradas:
        - html: Cadena que contiene el contenido HTML del que se extraen los enlaces

        Salidas:
        - Ninguna

        Restricciones:
        - La variable html no debe ser NULL
    */
    TidyDoc doc = tidyCreate();
    TidyBuffer buffer = {0};
    tidyBufInit(&buffer);

    tidyParseString(doc, html);
    tidyCleanAndRepair(doc);
    tidySaveBuffer(doc, &buffer);

    char *data = (char *)buffer.bp;

    regex_t regex_h2, regex_ol, regex_a;
    regmatch_t matches[2];

    const char *pattern_h2 = "<h2 id=\"books-last30\">";
    const char *pattern_ol = "<ol>";
    const char *pattern_a = "<a href=\"([^\"]+)\">";

    regcomp(&regex_h2, pattern_h2, REG_EXTENDED);
    regcomp(&regex_ol, pattern_ol, REG_EXTENDED);
    regcomp(&regex_a, pattern_a, REG_EXTENDED);

    if (regexec(&regex_h2, data, 0, NULL, 0) == 0) {
        char *pos = strstr(data, "<h2 id=\"books-last30\">");
        if (pos) {
            pos = strstr(pos, "<ol>");
            if (pos) {
                while (regexec(&regex_a, pos, 2, matches, 0) == 0) {
                    if (pos[matches[1].rm_so] == '#') {
                        break;
                    }

                    size_t length = matches[1].rm_eo - matches[1].rm_so;
                    char *link = malloc(length + strlen("https://www.gutenberg.org") + 1);
                    if (link) {
                        strcpy(link, "https://www.gutenberg.org");
                        strncat(link, pos + matches[1].rm_so, length);

                        FILE *file = fopen("links.txt", "a");
                        if (file) {
                            fprintf(file, "%s\n", link);
                            fclose(file);
                        } else {
                            perror("Error abriendo archivo links.txt");
                        }

                        free(link);
                    }
                    pos += matches[0].rm_eo;
                }
            }
        }
    }

    regfree(&regex_h2);
    regfree(&regex_ol);
    regfree(&regex_a);
    tidyBufFree(&buffer);
    tidyRelease(doc);
}

char *find_link(const char *html) {
    /*
        Función que encuentra un enlace en una cadena HTML que coincida con un patrón específico

        Entradas:
        - html: Cadena que contiene el contenido HTML donde se buscará el enlace

        Salidas:
        - link: Cadena que contiene el enlace encontrado. NULL si no se encuentra ningún enlace

        Restricciones:
        - La variable html no debe ser NULL
        - El patrón de búsqueda debe ser válido
    */
    regex_t regex;
    regmatch_t matches[2];
    const char *pattern = "href=\"(/ebooks/[^\"]+\\.txt\\.utf-8)\"";
    char *link = NULL;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "No se pudo compilar la expresión regular\n");
        return NULL;
    }

    if (regexec(&regex, html, 2, matches, 0) == 0) {
        size_t length = matches[1].rm_eo - matches[1].rm_so;
        link = malloc(length + 1);
        if (link) {
            strncpy(link, html + matches[1].rm_so, length);
            link[length] = '\0';
        }
    }

    regfree(&regex);
    return link;
}

void extract_title_and_author(const char *filename, char **title, char **author) {
    /*
        Función que extrae el título y el autor de un archivo de texto y los guarda en las variables proporcionadas

        Entradas:
        - filename: Nombre del archivo de texto del cual se extraerá el título y el autor
        - title: Puntero a una variable que recibirá el título del libro
        - author: Puntero a una variable que recibirá el nombre del autor

        Salidas:
        - Ninguna

        Restricciones:
        - La variable filename no debe ser NULL
        - Los punteros title y author deben ser inicializados en NULL
        - El archivo debe tener formato esperado con campos "Title:", "Author:", "Translator:", o "Editor:"
    */
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error abriendo archivo para extraer título y autor");
        return;
    }

    char buffer[1024];
    *title = NULL;
    *author = NULL;
    
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "Title:", 6) == 0) {
            *title = malloc(strlen(buffer) - 7 + 1);
            if (*title) {
                strcpy(*title, buffer + 7);
                (*title)[strcspn(*title, "\r\n")] = '\0';
            }
        } else if (strncmp(buffer, "Author:", 7) == 0) {
            *author = malloc(strlen(buffer) - 8 + 1);
            if (*author) {
                strcpy(*author, buffer + 8);
                (*author)[strcspn(*author, "\r\n")] = '\0';
            }
        } else if (strncmp(buffer, "Translator:", 11) == 0) {
            if (*author == NULL) {
                *author = malloc(strlen(buffer) - 12 + 1);
                if (*author) {
                    strcpy(*author, buffer + 12);
                    (*author)[strcspn(*author, "\r\n")] = '\0';
                }
            }
        } else if (strncmp(buffer, "Editor:", 7) == 0) {
            if (*author == NULL) {
                *author = malloc(strlen(buffer) - 8 + 1);
                if (*author) {
                    strcpy(*author, buffer + 8);
                    (*author)[strcspn(*author, "\r\n")] = '\0';
                }
            }
        }
    }

    fclose(file);
}

char *generate_unique_filename(const char *base_path) {
    /*
        Función que genera un nombre de archivo único basado en un nombre base

        Entradas:
        - base_path: Cadena que contiene la ruta base para el nombre del archivo

        Salidas:
        - unique_path: Cadena que contiene un nombre de archivo único generado

        Restricciones:
        - La variable base_path no debe ser NULL
    */
    char *unique_path = NULL;
    char buffer[256];
    int counter = 1;

    while (1) {
        snprintf(buffer, sizeof(buffer), "%s_%d.txt", base_path, counter);
        if (access(buffer, F_OK) != 0) { 
            unique_path = malloc(strlen(buffer) + 1);
            if (unique_path) {
                strcpy(unique_path, buffer);
            }
            break;
        }
        counter++;
    }

    return unique_path;
}

void process_link(CURL *curl_handle, const char *url) {
    /*
        Función que procesa un enlace utilizando CURL para descargar datos y luego los procesa

        Entradas:
        - curl_handle: Manejador de CURL ya inicializado
        - url: Cadena que contiene la URL del enlace a procesar

        Salidas:
        - Ninguna

        Restricciones:
        - La variable curl_handle debe ser un manejador de CURL válido
        - La variable url no debe ser NULL
    */
    CURLcode curl_status;
    memory_data response_chunk;
    response_chunk.data = malloc(1);
    response_chunk.size = 0;

    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&response_chunk);
    curl_status = curl_easy_perform(curl_handle);

    if (curl_status != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() falló para la URL %s: %s\n", url, curl_easy_strerror(curl_status));
    } else {
        char *link = find_link(response_chunk.data);
        if (link) {
            char full_url[256];
            snprintf(full_url, sizeof(full_url), "https://www.gutenberg.org%s", link);

            memory_data file_chunk;
            file_chunk.data = malloc(1);
            file_chunk.size = 0;

            curl_easy_setopt(curl_handle, CURLOPT_URL, full_url);
            curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&file_chunk);
            curl_status = curl_easy_perform(curl_handle);

            if (curl_status != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() falló para la URL %s: %s\n", full_url, curl_easy_strerror(curl_status));
            } else {
                const char *directory = "books_to_compress";
                char file_path[256];
                snprintf(file_path, sizeof(file_path), "%s/output.txt", directory);

                FILE *file = fopen(file_path, "w");
                if (file != NULL) {
                    fwrite(file_chunk.data, 1, file_chunk.size, file);
                    fclose(file);
                } else {
                    perror("Error abriendo el archivo para escribir el contenido descargado");
                }

                char *title = NULL;
                char *author = NULL;
                extract_title_and_author(file_path, &title, &author);
                if (title && author) {
                    char base_name[256];
                    snprintf(base_name, sizeof(base_name), "%s/%s - %s", directory, title, author);

                    char *unique_file_path = generate_unique_filename(base_name);
                    if (unique_file_path) {
                        if (remove(unique_file_path) != 0 && errno != ENOENT) {
                            perror("Error eliminando archivo existente");
                        }

                        if (rename("books_to_compress/output.txt", unique_file_path) == 0) {
                            printf("Se ha descargado: %s\n", unique_file_path);
                        } else {
                            perror("Error renombrando el archivo");
                        }

                        free(unique_file_path);
                    }

                    free(title);
                    free(author);
                } else {
                    fprintf(stderr, "No se pudo extraer título y autor en: %s\n", link);
                }

                free(file_chunk.data);
            }

            free(link);
        }
    }

    free(response_chunk.data);
}

int remove_directory(const char *dir_path) {
    /*
        Función que elimina un directorio y su contenido de forma recursiva

        Entradas:
        - dir_path: Cadena que contiene la ruta del directorio a eliminar

        Salidas:
        - 0 si la eliminación fue exitosa, -1 en caso de error

        Restricciones:
        - La variable dir_path no debe ser NULL
    */
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("Error abriendo directorio para eliminar");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        struct stat statbuf;
        if (stat(path, &statbuf) == -1) {
            perror("Error obteniendo información del archivo");
            closedir(dir);
            return -1;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (remove_directory(path) == -1) {
                closedir(dir);
                return -1;
            }
        } else {
            if (unlink(path) == -1) {
                perror("Error eliminando archivo");
                closedir(dir);
                return -1;
            }
        }
    }

    closedir(dir);
    return rmdir(dir_path);
}

void download_text_files(const char* base_url) {
    /*
        Función que descarga archivos de texto desde una URL base y los procesa

        Entradas:
        - base_url: Cadena que contiene la URL base desde donde se descargarán los archivos

        Salidas:
        - Ninguna

        Restricciones:
        - La variable base_url no debe ser NULL
    */
    CURL *curl;
    CURLcode res;
    memory_data chunk;

    chunk.data = malloc(1);  
    chunk.size = 0;      

    remove("links.txt");    

    const char *directory = "books_to_compress";
    struct stat dir_stat = {0};

    if (stat(directory, &dir_stat) != -1) {
        if (remove_directory(directory) == -1) {
            fprintf(stderr, "Error al eliminar el directorio %s\n", directory);
            return;
        }
    }

    if (mkdir(directory, 0700) != 0) {
        perror("Error creando el directorio books_to_compress");
        return;
    }

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, base_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() falló para la URL base %s: %s\n", base_url, curl_easy_strerror(res));
        } else {
            extract_links_from_html(chunk.data);
        }

        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "No se pudo inicializar CURL\n");
    }

    free(chunk.data);
    curl_global_cleanup();

    FILE *file;
    char url[256];

    curl = curl_easy_init();
    if (curl) {
        file = fopen("links.txt", "r");
        if (file == NULL) {
            perror("Error abriendo links.txt");
        } else {
            while (fgets(url, sizeof(url), file)) {
                url[strcspn(url, "\r\n")] = '\0';
                process_link(curl, url);
            }
            fclose(file);
        }

        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "No se pudo inicializar CURL para el procesamiento de enlaces\n");
    }

    curl_global_cleanup();
    remove("links.txt");
}