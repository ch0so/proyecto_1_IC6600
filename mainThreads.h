#ifndef HUFFMAN_COMPRESSION_H
#define HUFFMAN_COMPRESSION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Constants
#define ENCODED_DATA_THREADS 8
#define FILE_PATH_SIZE 256

// Struct definitions
typedef struct args_thread {
    char file_path[FILE_PATH_SIZE];
    char file_name[FILE_PATH_SIZE];
    FILE *compressed_file;
    int thread_id;
} args_thread;

typedef struct encoded_data_threads {
    char *encoded_str;
    size_t encoded_len;
} encoded_data_threads;

typedef struct huffman_node {
    int frequency;
    char character;
    struct huffman_node *left;
    struct huffman_node *right;
} huffman_node;

typedef struct code_map {
    char character;
    char *code;
} code_map;

typedef struct huffman_encoded_data {
    const char *str;
    code_map *map;
    int map_size;
    char *encoded_str;
    long initial;
    long final;
    size_t buffer_size;
} huffman_encoded_data;

typedef struct huffman_decoded_data {
    const char *encoded_str;
    long initial;
    long final;
    huffman_node *root;
    FILE *output_file;
    pthread_mutex_t *mutex;
} huffman_decoded_data;

// Mutex
extern pthread_mutex_t encoded_str_mutex;
extern pthread_mutex_t file_mutex;
extern pthread_mutex_t decode_str_mutex;

// Function declarations
void download_text_files(const char *base_url);
int compare_nodes(const void *a, const void *b);
huffman_node *create_node(char character, int frequency);
void count_frequencies(const char *str, int *frequencies, char *unique_characters, int *unique_count);
huffman_node *build_huffman_tree(int node_count, huffman_node **nodes);
void *encode_huffman_aux(void *arg);
void encode_huffman_threads(const char *str, code_map *map, int map_size, char **encoded_str, size_t *encoded_len);
void *decode_huffman_aux(void *arg);
void decode_huffman_threads(const char *encoded_str, huffman_node *root, FILE *output_file);
void generate_codes(huffman_node *root, char *code, int length, code_map *map, int *map_size);
void create_code_map(huffman_node *root, code_map *map, int *map_size);
void free_huffman_tree(huffman_node *root);
char *read_file(const char *file_path);
void write_binary_file(const char *file_path, const char *encoded_str);
char *read_binary_file(const char *file_path, size_t *length);
void write_huffman_tree(FILE *file, huffman_node *root);
huffman_node *read_huffman_tree(FILE *file);
void write_bits(FILE *file, const char *bits, size_t len);
void process_directory(const char *dir_path, const char *compressed_file_path);
void *thread_process(void *arg);
void process_directory_threads(const char *dir_path, const char *compressed_file_path);
void read_bits(FILE *file, char *bits, size_t len);
void decompress_files(const char *compressed_file_path, const char *output_dir);

#endif // HUFFMAN_COMPRESSION_H
