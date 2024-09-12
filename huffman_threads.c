#include <sys/stat.h>
#include "web_crawler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "huffman_threads.h"

pthread_mutex_t encoded_str_mutex;
pthread_mutex_t file_mutex;
pthread_mutex_t decode_str_mutex;


int compare_nodes(const void *a, const void *b)
{
    huffman_node *node_a = *(huffman_node **)a;
    huffman_node *node_b = *(huffman_node **)b;
    return node_a->frequency - node_b->frequency;
}

huffman_node *create_node(char character, int frequency)
{
    huffman_node *node = (huffman_node *)malloc(sizeof(huffman_node));
    node->character = character;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void count_frequencies(const char *str, int *frequencies, char *unique_characters, int *unique_count)
{
    int count[256] = {0};
    const char *ptr = str;

    while (*ptr)
    {
        count[(unsigned char)*ptr]++;
        ptr++;
    }

    *unique_count = 0;
    for (int i = 0; i < 256; i++)
    {
        if (count[i] > 0)
        {
            unique_characters[*unique_count] = (char)i;
            frequencies[*unique_count] = count[i];
            (*unique_count)++;
        }
    }
}

huffman_node *build_huffman_tree(int node_count, huffman_node **nodes)
{
    qsort(nodes, node_count, sizeof(huffman_node *), compare_nodes);

    while (node_count > 1) { 
        huffman_node *left = nodes[0];
        huffman_node *right = nodes[1];

        huffman_node *parent = create_node('-', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;

        for (int i = 2; i < node_count; i++)
        {
            nodes[i - 2] = nodes[i];
        }
        nodes[node_count - 2] = parent;

        node_count--;
        qsort(nodes, node_count, sizeof(huffman_node *), compare_nodes);
    }

    return nodes[0];
}

void *encode_huffman_aux(void *arg) 
{
    huffman_encoded_data *data = (huffman_encoded_data *)arg;

    const char *str = data->str + data->initial;
    code_map *map = data->map;
    int map_size = data->map_size;

    size_t buffer_size = 1024;
    char *thread_encoded_str = (char *)malloc(buffer_size);

    size_t pos = 0;

    for (long i = data->initial; i < data->final; i++, str++)
    {
        int found = 0;
        for (int j = 0; j < map_size; j++)
        {
            if (*str == map[j].character)
            {
                size_t code_len = strlen(map[j].code);
                while (pos + code_len >= buffer_size)
                {
                    buffer_size *= 2;
                    thread_encoded_str = (char *)realloc(thread_encoded_str, buffer_size);
                    if (!thread_encoded_str)
                    {
                        perror("Error reallocating memory in thread");
                        pthread_exit(NULL);
                    }
                }
                strcpy(thread_encoded_str + pos, map[j].code);
                pos += code_len;
                found = 1;
                break;
            }
        }
        if (!found)
        {
            fprintf(stderr, "Error: Character '%c' not found in the code map\n", *str);
            free(thread_encoded_str);
            pthread_exit(NULL);
        }
    }

    thread_encoded_str[pos] = '\0';

    data->buffer_size = buffer_size;
    data->encoded_str = thread_encoded_str;

    pthread_exit(NULL);
}

void encode_huffman_threads(const char *str, code_map *map, int map_size, char **encoded_str, size_t *encoded_len)
{
    long string_length = strlen(str);
    long segment_length = string_length / ENCODED_DATA_THREADS;
    

    pthread_t threads[ENCODED_DATA_THREADS];
    huffman_encoded_data thread_data[ENCODED_DATA_THREADS];

    pthread_mutex_init(&encoded_str_mutex, NULL);
    *encoded_len = 0;

    for (int i = 0; i < ENCODED_DATA_THREADS; i++)
    {
        thread_data[i].str = str;
        thread_data[i].map = map;
        thread_data[i].map_size = map_size;
        thread_data[i].initial = i * segment_length;
        thread_data[i].final = (i == ENCODED_DATA_THREADS - 1) ? string_length : (i + 1) * segment_length;

        pthread_create(&threads[i], NULL, encode_huffman_aux, (void *)&thread_data[i]);
    }

    for (int i = 0; i < ENCODED_DATA_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    size_t total_size = 0;
    for (int i = 0; i < ENCODED_DATA_THREADS; i++)
    {
        total_size += strlen(thread_data[i].encoded_str);
    }

    *encoded_str = (char *)malloc(total_size + 1);  
    (*encoded_str)[0] = '\0';

    for (int i = 0; i < ENCODED_DATA_THREADS; i++)
    {
        strcat(*encoded_str, thread_data[i].encoded_str);
        *encoded_len += strlen(thread_data[i].encoded_str);
        free(thread_data[i].encoded_str);
    }

    pthread_mutex_destroy(&encoded_str_mutex);
}

void *decode_huffman_aux(void *arg)
{
    huffman_decoded_data *data = (huffman_decoded_data *)arg;
    huffman_node *current = data->root;

    for (long i = data->initial; i < data->final; i++)
    {
        if (data->encoded_str[i] == '0')
        {
            current = current->left;
        }
        else if (data->encoded_str[i] == '1')
        {
            current = current->right;
        }

        if (current->left == NULL && current->right == NULL)
        {

            pthread_mutex_lock(data->mutex);
            fputc(current->character, data->output_file);
            pthread_mutex_unlock(data->mutex);

            current = data->root;
        }
    }

    return NULL;
}

void decode_huffman_threads(const char *encoded_str, huffman_node *root, FILE *output_file)
{
    long string_length = strlen(encoded_str);
    long segment_length = string_length / ENCODED_DATA_THREADS;

    pthread_t threads[ENCODED_DATA_THREADS];
    huffman_decoded_data thread_data[ENCODED_DATA_THREADS];
    pthread_mutex_init(&decode_str_mutex, NULL);

    for (int i = 0; i < ENCODED_DATA_THREADS; i++)
    {
        thread_data[i].encoded_str = encoded_str;
        thread_data[i].initial = i * segment_length;
        thread_data[i].final = (i == ENCODED_DATA_THREADS - 1) ? string_length : (i + 1) * segment_length;
        thread_data[i].root = root;
        thread_data[i].output_file = output_file;
        thread_data[i].mutex = &decode_str_mutex;

        pthread_create(&threads[i], NULL, decode_huffman_aux, &thread_data[i]);
    }

    for (int i = 0; i < ENCODED_DATA_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&decode_str_mutex);
}

void generate_codes(huffman_node *root, char *code, int length, code_map *map, int *map_size)
{
    if (root == NULL)
    {
        return;
    }

    if (root->left == NULL && root->right == NULL)
    {
        map[*map_size].character = root->character;
        map[*map_size].code = (char *)malloc(length + 1);
        if (map[*map_size].code == NULL)
        {
            perror("Error asignando memoria para el cÃ³digo");
            exit(EXIT_FAILURE);
        }
        strncpy(map[*map_size].code, code, length);
        map[*map_size].code[length] = '\0';
        (*map_size)++;
    }

    if (root->left)
    {
        code[length] = '0';
        generate_codes(root->left, code, length + 1, map, map_size);
    }
    if (root->right)
    {
        code[length] = '1';
        generate_codes(root->right, code, length + 1, map, map_size);
    }
    code[length] = '\0';
}

void create_code_map(huffman_node *root, code_map *map, int *map_size)
{
    char code[256];
    *map_size = 0;
    generate_codes(root, code, 0, map, map_size);
}

void free_huffman_tree(huffman_node *root)
{
    if (root == NULL)
    {
        return;
    }

    free_huffman_tree(root->left);
    free_huffman_tree(root->right);

    free(root);
}

char *read_file(const char *file_path)
{
    FILE *file = fopen(file_path, "rb");
    if (!file)
    {
        perror("Error abriendo el archivo");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer)
    {
        perror("Error asignando memoria");
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}

void write_huffman_tree(FILE *file, huffman_node *root)
{
    if (root == NULL)
    {
        fputc(0, file);
        return;
    }

    fputc(1, file);

    unsigned char char_byte = root->character;
    for (int bit = 7; bit >= 0; bit--)
    {
        fputc((char_byte & (1 << bit)) ? '1' : '0', file);
    }

    unsigned char freq_byte[sizeof(int) * 8] = {0};
    for (int i = 0; i < sizeof(int) * 8; i++)
    {
        freq_byte[i] = (root->frequency & (1 << (sizeof(int) * 8 - 1 - i))) ? '1' : '0';
    }
    for (int i = 0; i < sizeof(int) * 8; i++)
    {
        fputc(freq_byte[i], file);
    }
    // [DIVIDIR]
    write_huffman_tree(file, root->left);
    write_huffman_tree(file, root->right);
}

huffman_node *read_huffman_tree(FILE *file)
{
    int is_non_null = fgetc(file);
    if (is_non_null == 0)
    {
        return NULL;
    }

    huffman_node *node = (huffman_node *)malloc(sizeof(huffman_node));
    if (node == NULL)
    {
        perror("Error allocating memory for node");
        exit(EXIT_FAILURE);
    }

    unsigned char char_byte = 0;
    for (int bit = 7; bit >= 0; bit--)
    {
        int bit_value = fgetc(file) == '1' ? 1 : 0;
        char_byte |= (bit_value << bit);
    }
    node->character = char_byte;

    node->frequency = 0;
    for (int i = 0; i < sizeof(int) * 8; i++)
    {
        int bit_value = fgetc(file) == '1' ? 1 : 0;
        node->frequency |= (bit_value << (sizeof(int) * 8 - 1 - i));
    }

    // [DIVIDIR]
    node->left = read_huffman_tree(file);
    node->right = read_huffman_tree(file);

    return node;
}

void write_bits(FILE *file, const char *bits, size_t len)
{
    unsigned char byte = 0;
    int bit_count = 0;

    for (size_t i = 0; i < len; i++)
    {
        if (bits[i] == '1')
        {
            byte |= (1 << (7 - bit_count));
        }
        bit_count++;

        if (bit_count == 8)
        {
            fwrite(&byte, sizeof(unsigned char), 1, file);
            byte = 0;
            bit_count = 0;
        }
    }

    if (bit_count > 0)
    {
        fwrite(&byte, sizeof(unsigned char), 1, file);
    }
}

void process_directory(const char *dir_path, const char *compressed_file_path)
{
    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        perror("Error abriendo el directorio");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    FILE *compressed_file = fopen(compressed_file_path, "wb");
    if (!compressed_file)
    {
        perror("Error abriendo el archivo comprimido para escritura");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            size_t path_len = strlen(dir_path) + strlen(entry->d_name) + 2;
            char *input_file_path = (char *)malloc(path_len);
            if (input_file_path == NULL)
            {
                perror("Error asignando memoria");
                exit(EXIT_FAILURE);
            }
            snprintf(input_file_path, path_len, "%s/%s", dir_path, entry->d_name);

            char *input_str = read_file(input_file_path);

            int frequencies[256];
            char unique_characters[256];
            int unique_count;
            count_frequencies(input_str, frequencies, unique_characters, &unique_count);

            huffman_node *nodes[256];
            for (int i = 0; i < unique_count; i++)
            { // [DIVIDIR] en 27 hilos para cada letra (Se tendria que hacer varios for por el indice)
                nodes[i] = create_node(unique_characters[i], frequencies[i]);
            }

            huffman_node *root = build_huffman_tree(unique_count, nodes);

            code_map map[256];
            int map_size;
            create_code_map(root, map, &map_size);

            char *encoded_str;
            size_t encoded_len;
            encode_huffman_threads(input_str, map, map_size, &encoded_str, &encoded_len);

            size_t name_len = strlen(entry->d_name);
            fwrite(&name_len, sizeof(size_t), 1, compressed_file);
            fwrite(entry->d_name, sizeof(char), name_len, compressed_file);

            write_huffman_tree(compressed_file, root);

            fwrite(&encoded_len, sizeof(size_t), 1, compressed_file);
            write_bits(compressed_file, encoded_str, encoded_len);

            for (int i = 0; i < map_size; i++)
            {
                free(map[i].code);
            }
            free(input_str);
            free(encoded_str);
            free_huffman_tree(root);
            free(input_file_path);
        }
    }

    closedir(dir);
    fclose(compressed_file);
}

void *thread_process(void *arg)
{
    args_thread *args = (args_thread *)arg;

    char *input_str = read_file(args->file_path);
    int frequencies[256];
    char unique_characters[256];
    int unique_count;
    count_frequencies(input_str, frequencies, unique_characters, &unique_count);

    huffman_node *nodes[256];
    for (int i = 0; i < unique_count; i++)
    {
        nodes[i] = create_node(unique_characters[i], frequencies[i]);
    }

    huffman_node *root = build_huffman_tree(unique_count, nodes);

    code_map map[256];
    int map_size;
    create_code_map(root, map, &map_size);

    char *encoded_str;
    size_t encoded_len;
    encode_huffman_threads(input_str, map, map_size, &encoded_str, &encoded_len);

    pthread_mutex_lock(&file_mutex);
    size_t name_len = strlen(args->file_name);
    fwrite(&name_len, sizeof(size_t), 1, args->compressed_file); // Escribe tam de nombre
    fwrite(args->file_name, sizeof(char), name_len, args->compressed_file); // Escribe nombre

    write_huffman_tree(args->compressed_file, root); // Escribe arbol
    fwrite(&encoded_len, sizeof(size_t), 1, args->compressed_file); // Escribe tam de encoded
    write_bits(args->compressed_file, encoded_str, encoded_len); // Escribe encoded
    pthread_mutex_unlock(&file_mutex);

    for (int i = 0; i < map_size; i++) {
        free(map[i].code);
    }

    free(input_str);
    free(encoded_str);
    free_huffman_tree(root);

    free(args);

    return NULL;
};

void process_directory_threads(const char *dir_path, const char *compressed_file_path)
{
    pthread_mutex_init(&file_mutex, NULL);
    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        perror("Error abriendo el directorio");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    FILE *compressed_file = fopen(compressed_file_path, "wb");
    if (!compressed_file)
    {
        perror("Error abriendo el archivo comprimido para escritura");
        exit(EXIT_FAILURE);
    }

    int amount_files;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            amount_files++;
        }
    }
    closedir(dir);

    pthread_t threads[amount_files];
    args_thread *arr_args[amount_files];

    dir = opendir(dir_path);
    if (dir == NULL)
    {
        perror("Error abriendo el directorio");
        exit(EXIT_FAILURE);
    }

    int thread_counter = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            args_thread *args = (args_thread *)malloc(sizeof(args_thread));
            if (args == NULL)
            {
                printf("Error assigning memory to thread arg");
                exit(EXIT_FAILURE);
            }

            snprintf(args->file_path, sizeof(args->file_path), "%s/%s", dir_path, entry->d_name);
            args->thread_id = thread_counter;
            strncpy(args->file_name, entry->d_name, sizeof(args->file_name) - 1);
            args->compressed_file = compressed_file;

            arr_args[thread_counter] = args;
            pthread_create(&threads[thread_counter], NULL, thread_process, args);

            thread_counter++;
        }
    }

    for (int i = 0; i < thread_counter; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&file_mutex); 

    closedir(dir);
    fclose(compressed_file);
}

void read_bits(FILE *file, char *bits, size_t len)
{
    unsigned char byte;
    int bit_count = 0;
    size_t bit_index = 0;

    while (bit_index < len)
    {
        if (bit_count == 0)
        {
            fread(&byte, sizeof(unsigned char), 1, file);
        }

        bits[bit_index] = ((byte >> (7 - bit_count)) & 1) ? '1' : '0';
        bit_count++;
        bit_index++;

        if (bit_count == 8)
        {
            bit_count = 0;
        }
    }
}

void decompress_files(const char *compressed_file_path, const char *output_dir)
{
    FILE *compressed_file = fopen(compressed_file_path, "rb");
    if (!compressed_file)
    {
        perror("Error abriendo el archivo comprimido para lectura");
        exit(EXIT_FAILURE);
    }

    struct stat st = {0};
    if (stat(output_dir, &st) == -1)
    {
        mkdir(output_dir, 0700);
    }

    while (1)
    {
        size_t name_len;
        if (fread(&name_len, sizeof(size_t), 1, compressed_file) != 1)
            break;

        char *file_name = (char *)malloc(2048 + 1);
        if (file_name == NULL)
        {
            perror("Error asignando memoria para el file_name");
            exit(EXIT_FAILURE);
        }
        fread(file_name, sizeof(char), name_len, compressed_file);
        file_name[name_len] = '\0';

        huffman_node *root = read_huffman_tree(compressed_file);

        size_t encoded_len;
        fread(&encoded_len, sizeof(size_t), 1, compressed_file);
        char *encoded_str = (char *)malloc(encoded_len + 1);
        if (encoded_str == NULL)
        {
            perror("Error asignando memoria para el encoded_str");
            exit(EXIT_FAILURE);
        }
        encoded_str[encoded_len] = '\0';
        read_bits(compressed_file, encoded_str, encoded_len);

        size_t path_len = strlen(output_dir) + strlen(file_name) + 2;
        char *output_file_path = (char *)malloc(path_len);
        if (output_file_path == NULL)
        {
            perror("Error asignando memoria para el output_file_path");
            exit(EXIT_FAILURE);
        }
        snprintf(output_file_path, path_len, "%s/%s", output_dir, file_name);
        FILE *output_file = fopen(output_file_path, "wb");
        if (!output_file)
        {
            perror("Error abriendo el archivo de salida");
            exit(EXIT_FAILURE);
        }

        decode_huffman_threads(encoded_str, root, output_file);
        fclose(output_file);

        free(file_name);
        free(encoded_str);
        free(output_file_path);
        free_huffman_tree(root);
    }

    fclose(compressed_file);
}

int main()
{
    // const char* base_url = "https://www.gutenberg.org/browse/scores/top";
    // download_text_files(base_url);
    const char *input_dir = "books_to_compress";
    const char *compressed_file_path = "compressed_books/compressed.bin";
    const char *decompressed_dir = "decompressed_books";

    int n = 100; // Number of iterations to run for better accuracy
    clock_t start, end;
    double totalTimeCompress = 0.0, totalTimeDecompress = 0.0;
    
    printf("Execution time for compression and decompression of text files\n");
    printf("Threads Implementation\n");
    printf("Number of threads: %d\n", ENCODED_DATA_THREADS);
    printf("Number of iterations: %d\n\n", n);
    printf("------------------------------------------------------------\n");

    // Time the process_directory_threads function n times
    for (int i = 0; i < n; i++) {
        start = clock();
        process_directory_threads(input_dir, compressed_file_path);
        end = clock();
        totalTimeCompress += ((double)(end - start)) / CLOCKS_PER_SEC;
    }

    // Calculate the average time for compression
    double averageCompressTime = totalTimeCompress / n;
    printf("Total time taken to compress: %f seconds\n", totalTimeCompress);
    printf("Average time taken to compress: %f seconds\n", averageCompressTime);

    // Time the decompress_files function n times
    for (int i = 0; i < n; i++) {
        start = clock();
        decompress_files(compressed_file_path, decompressed_dir);
        end = clock();
        totalTimeDecompress += ((double)(end - start)) / CLOCKS_PER_SEC;
    }

    // Calculate the average time for decompression
    double averageDecompressTime = totalTimeDecompress / n;
    printf("Total time taken to decompress: %f seconds\n", totalTimeDecompress);
    printf("Average time taken to decompress: %f seconds\n", averageDecompressTime);

    return 0;
}