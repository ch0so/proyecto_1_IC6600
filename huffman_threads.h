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

Referencias bibliográficas:
    - GeeksforGeeks. (2023a, abril 7). Huffman Decoding [Decodificación de Huffman]. GeeksforGeeks. https://www.geeksforgeeks.org/huffman-decoding/ 
    - GeeksforGeeks. (2023b, setiembre 11). Huffman Coding [Codificación de Huffman]. GeeksforGeeks. https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/ 
    - GeeksforGeeks. (2023c, 9 mayo). Thread functions in C/C++ [Funciones de Hilos en C/C++]. GeeksforGeeks. https://www.geeksforgeeks.org/thread-functions-in-c-c/
    - IBM. (2023, marzo 24). Using mutexes [Uso de Mutexes]. IBM. https://www.ibm.com/docs/pt-br/aix/7.2?topic=programming-using-mutexes
*/
#ifndef HUFFMAN_THREADS_H
#define HUFFMAN_THREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "huffman_serial.h"

// Número de hilos que se utilizarán para la codificación
#define ENCODED_DATA_THREADS 8

// Tamaño máximo de la ruta de archivo
#define FILE_PATH_SIZE 290

// Capacidad inicial para arrays dinámicos que almacenan archivos y posiciones
#define INITIAL_CAPACITY 256

// Estructura que contiene los argumentos necesarios para que un hilo procese un archivo comprimido
typedef struct decompress_args {
    const char* compressed_file_path; // Ruta del archivo comprimido
    const char* file_name; // Nombre del archivo de salida
    const char* output_file_path; // Ruta del archivo de salida
    long long int position; // Posición en el archivo comprimido desde donde comenzar la descompresión
    huffman_node* huffman_tree; // Árbol de Huffman para la descompresión
    char* encoded_str; // Cadena codificada para la descompresión
} decompress_args_t;

// Estructura que contiene los argumentos necesarios para que un hilo procese un archivo
typedef struct args_thread {
    char file_path[FILE_PATH_SIZE]; // Ruta completa del archivo a procesar
    char file_name[FILE_PATH_SIZE]; // Nombre del archivo a procesar
    FILE *compressed_file; // Puntero al archivo comprimido donde se escribirá la salida
    int thread_id; // Identificador del hilo que procesará el archivo
} args_thread;

// Estructura que contiene los datos codificados por Huffman para un archivo
typedef struct encoded_data_threads {
    char *encoded_str; // Cadena codificada con Huffman
    size_t encoded_len; // Longitud de la cadena codificada
} encoded_data_threads;

// Estructura que contiene los datos necesarios para la codificación Huffman
typedef struct huffman_encoded_data {
    const char *str; // Cadena de caracteres a codificar
    code_map *map; // Mapa de códigos de Huffman para los caracteres
    int map_size; // Tamaño del mapa de códigos
    char *encoded_str; // Cadena resultante codificada en Huffman
    long initial; // Posición inicial en el buffer para la codificación
    long final; // Posición final en el buffer para la codificación
    size_t buffer_size; // Tamaño del buffer de codificación
} huffman_encoded_data;

// Estructura que contiene los datos necesarios para la decodificación Huffman
typedef struct huffman_decoded_data {
    const char *encoded_str; // Cadena codificada en Huffman que se debe decodificar
    long initial; // Posición inicial en el buffer para la decodificación
    long final; // Posición final en el buffer para la decodificación
    huffman_node *root; // Raíz del árbol de Huffman utilizado para la decodificación
    FILE *output_file; // Archivo donde se escribirá la cadena decodificada
    pthread_mutex_t *mutex; // Mutex para asegurar la exclusión mutua al escribir en el archivo de salida
} huffman_decoded_data;

// Mutex para sincronizar el acceso a la cadena codificada
extern pthread_mutex_t encoded_str_mutex;

// Mutex para sincronizar el acceso al archivo comprimido
extern pthread_mutex_t file_mutex;

// Mutex para sincronizar el acceso a la cadena decodificada
extern pthread_mutex_t decode_str_mutex;

// Función que compara dos nodos de Huffman en función de su frecuencia
int compare_nodes_threads(const void* first_node, const void* second_node);

// Función que crea un nuevo nodo de Huffman
huffman_node* create_node_threads(char character, int frequency);

// Función que cuenta las frecuencias de los caracteres en una cadena de texto
void count_frequencies_threads(const char* string, int* frequencies, char* unique_characters, int* unique_count);

// Función que construye un árbol de Huffman a partir de una lista de nodos
huffman_node* build_huffman_tree_threads(int node_count, huffman_node** nodes);

// Función auxiliar que codifica una cadena utilizando el algoritmo de Huffman en múltiples hilos
void *encode_huffman_threads_aux(void *arg);

// Función que divide la cadena a codificar y utiliza hilos para paralelizar el proceso de codificación de Huffman
void encode_huffman_threads(const char *str, code_map *map, int map_size, char **encoded_str, size_t *encoded_len);

// Función que divide la cadena codificada y utiliza hilos para paralelizar el proceso de decodificación de Huffman
void decode_huffman_threads(const char* encoded_str, huffman_node* root, FILE* output_file);

// Función que genera códigos de Huffman para cada carácter en el árbol
void generate_codes_threads(huffman_node* root, char* code, int length, code_map* map, int* map_size);

// Función que crea un mapa de códigos de Huffman a partir de un árbol
void create_code_map_threads(huffman_node* root, code_map* map, int* map_size);

// Función que libera la memoria utilizada por un árbol de Huffman
void free_huffman_tree_threads(huffman_node* root);

// Función que lee el contenido de un archivo en una cadena de texto
char* read_file_threads(const char* file_path);

// Función que escribe un árbol de Huffman en un archivo
void write_huffman_tree_threads(FILE* file, huffman_node* root);

// Función que lee un árbol de Huffman desde un archivo
huffman_node* read_huffman_tree_threads(FILE* file);

// Función que escribe una cadena de bits en un archivo
void write_bits_threads(FILE* file, const char* bits, size_t len);

// Función auxiliar que procesa un archivo individual utilizando múltiples hilos para la compresión con Huffman
void *thread_process(void *arg);

// Función que comprime múltiples archivos de un directorio utilizando hilos para paralelizar la compresión con Huffman
void compress_files_threads(const char *dir_path, const char *compressed_file_path);

// Función que lee una cadena de bits de un archivo
void read_bits_threads(FILE* file, char* bits, size_t len);

// Función que descomprime todos los archivos desde un archivo comprimido y guarda los resultados en un directorio
void decompress_files_threads(const char *compressed_file_path, const char *output_dir);


#endif // HUFFMAN_THREADS_H
