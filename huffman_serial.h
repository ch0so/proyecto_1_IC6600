#ifndef HUFFMAN_SERIAL_H
#define HUFFMAN_SERIAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct huffman_node {
    int frequency;
    char character;
    struct huffman_node* left;
    struct huffman_node* right;
} huffman_node;

typedef struct code_map {
    char character;
    char* code;
} code_map;

// Función que compara dos nodos Huffman basándose en la frecuencia de sus caracteres
int compare_nodes(const void* a, const void* b);

// Crea un nuevo nodo Huffman
huffman_node* create_node(char character, int frequency);

// Cuenta las frecuencias de los caracteres en una cadena
void count_frequencies(const char* str, int* frequencies, char* unique_characters, int* unique_count);

// Construye el árbol Huffman a partir de un conjunto de nodos
huffman_node* build_huffman_tree(int node_count, huffman_node** nodes);

// Codifica una cadena usando el mapa de códigos de Huffman
void encode_huffman(const char* str, code_map* map, int map_size, char** encoded_str, size_t* encoded_len);

// Decodifica una cadena codificada con Huffman, escribiendo la salida en un archivo
void decode_huffman(const char* encoded_str, huffman_node* root, FILE* output_file);

// Genera códigos Huffman para un nodo
void generate_codes(huffman_node* root, char* code, int length, code_map* map, int* map_size);

// Crea el mapa de códigos Huffman a partir del árbol Huffman
void create_code_map(huffman_node* root, code_map* map, int* map_size);

// Libera la memoria asociada a un árbol Huffman
void free_huffman_tree(huffman_node* root);

// Lee el contenido de un archivo en memoria
char* read_file(const char* file_path);

// Escribe un árbol Huffman en un archivo
void write_huffman_tree(FILE* file, huffman_node* root);

// Lee un árbol Huffman desde un archivo
huffman_node* read_huffman_tree(FILE* file);

// Escribe bits en un archivo
void write_bits(FILE* file, const char* bits, size_t len);

// Procesa un directorio, comprimiendo cada archivo y escribiendo los datos comprimidos en un archivo de salida
void process_directory(const char* dir_path, const char* compressed_file_path);

// Lee bits desde un archivo
void read_bits(FILE* file, char* bits, size_t len);

// Descomprime los archivos de un archivo comprimido y los escribe en un directorio de salida
void decompress_files(const char* compressed_file_path, const char* output_dir);

#endif // HUFFMAN_SERIAL_H