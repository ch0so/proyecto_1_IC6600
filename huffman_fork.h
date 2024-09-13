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
-GeeksforGeeks. (2023a, abril 7). Huffman Decoding [Decodificación de Huffman]. GeeksforGeeks. https://www.geeksforgeeks.org/huffman-decoding/ 
-GeeksforGeeks. (2023b, setiembre 11). Huffman Coding [Codificación de Huffman]. GeeksforGeeks. https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/ 
-Hu, J. (2023, 12 octubre). Usa la función fork en C. Delft Stack. https://www.delftstack.com/es/howto/c/fork-in-c/
*/

#ifndef HUFFMAN_FORK_H
#define HUFFMAN_FORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

// Prefijo para los archivos temporales generados durante la compresión
#define TEMP_FILE_PREFIX "tempfile_"
// Capacidad inicial para arrays dinámicos que almacenan archivos y posiciones
#define INITIAL_CAPACITY 256

// Nodo de un árbol de Huffman
typedef struct huffman_node {
    int frequency; // Frecuencia del carácter en el archivo
    char character; // Carácter asociado a este nodo
    struct huffman_node* left; // Hijo izquierdo del nodo
    struct huffman_node* right; // Hijo derecho del nodo
} huffman_node;

// Mapa de caracteres a códigos binarios
typedef struct code_map {
    char character; // Carácter que se codifica
    char* code; // Código binario asociado al carácter
} code_map;

// Función que compara dos nodos de Huffman en función de su frecuencia
int compare_nodes_fork(const void* first_node, const void* second_node);

// Función que crea un nuevo nodo de Huffman
huffman_node* create_node_fork(char character, int frequency);

// Función que cuenta las frecuencias de los caracteres en una cadena de texto
void count_frequencies_fork(const char* string, int* frequencies, char* unique_characters, int* unique_count);

// Función que construye un árbol de Huffman a partir de una lista de nodos
huffman_node* build_huffman_tree_fork(int node_count, huffman_node** nodes);

// Función que codifica una cadena de texto usando un mapa de códigos de Huffman
void encode_huffman_fork(const char* string_to_encode, code_map* map, int map_size, char** encoded_str, size_t* encoded_len);

// Función que decodifica una cadena codificada usando un árbol de Huffman
void decode_huffman_fork(const char* encoded_str, huffman_node* root, FILE* output_file);

// Función que genera códigos de Huffman para cada carácter en el árbol
void generate_codes_fork(huffman_node* root, char* code, int length, code_map* map, int* map_size);

// Función que crea un mapa de códigos de Huffman a partir de un árbol
void create_code_map_fork(huffman_node* root, code_map* map, int* map_size);

// Función que libera la memoria utilizada por un árbol de Huffman
void free_huffman_tree_fork(huffman_node* root);

// Función que lee el contenido de un archivo en una cadena de texto
char* read_file_fork(const char* file_path);

// Función que escribe un árbol de Huffman en un archivo
void write_huffman_tree_fork(FILE* file, huffman_node* root);

// Función que lee un árbol de Huffman desde un archivo
huffman_node* read_huffman_tree_fork(FILE* file);

// Función que escribe una cadena de bits en un archivo
void write_bits_fork(FILE* file, const char* bits, size_t len);

// Función que comprime un archivo de texto y lo guarda en un archivo temporal
void compress_file_fork(const char* input_file_path, const char* temp_file_path);

// Función que comprime todos los archivos en un directorio y los guarda en un archivo comprimido
void compress_files_fork(const char* dir_path, const char* compressed_file_path);

// Función que lee una cadena de bits de un archivo
void read_bits_fork(FILE* file, char* bits, size_t len);

// Función que descomprime todos los archivos desde un archivo comprimido y guarda los resultados en un directorio
void decompress_file_fork(const char* compressed_file_path, const char* file_name, const char* output_file_path, long long int position);

// Función que descomprime todos los archivos desde un archivo comprimido y guarda los resultados en un directorio
void decompress_files_fork(const char* compressed_file_path, const char* output_dir);

#endif // HUFFMAN_FORK_H