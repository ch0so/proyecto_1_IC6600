#include <sys/stat.h>
#include "web_crawler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void download_text_files(const char* base_url) {
    CURL *curl;
    CURLcode res;
    memory_data chunk;

    chunk.data = malloc(1);  
    chunk.size = 0;          

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, base_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            extract_links_from_html(chunk.data);
        }

        curl_easy_cleanup(curl);
    }

    free(chunk.data);
    curl_global_cleanup();

    FILE *file;
    char url[256];

    curl = curl_easy_init();
    if (curl) {
        file = fopen("links.txt", "r");
        if (file == NULL) {
            perror("Error opening links.txt");
        }

        while (fgets(url, sizeof(url), file)) {
            url[strcspn(url, "\r\n")] = '\0';
            process_link(curl, url);
        }

        fclose(file);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    remove("links.txt");
}

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

int compare_nodes(const void* a, const void* b) {
    huffman_node* node_a = *(huffman_node**)a;
    huffman_node* node_b = *(huffman_node**)b;
    return node_a->frequency - node_b->frequency;
}

huffman_node* create_node(char character, int frequency) {
    huffman_node* node = (huffman_node*)malloc(sizeof(huffman_node));
    node->character = character;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void count_frequencies(const char* str, int* frequencies, char* unique_characters, int* unique_count) {
    int count[256] = {0};
    const char* ptr = str;

    while (*ptr) {
        count[(unsigned char)*ptr]++;
        ptr++;
    }

    *unique_count = 0;
    for (int i = 0; i < 256; i++) {
        if (count[i] > 0) {
            unique_characters[*unique_count] = (char)i;
            frequencies[*unique_count] = count[i];
            (*unique_count)++;
        }
    }
}

huffman_node* build_huffman_tree(int node_count, huffman_node** nodes) {
    qsort(nodes, node_count, sizeof(huffman_node*), compare_nodes);

    while (node_count > 1) {
        huffman_node* left = nodes[0];
        huffman_node* right = nodes[1];

        huffman_node* parent = create_node('-', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;

        for (int i = 2; i < node_count; i++) {
            nodes[i - 2] = nodes[i];
        }
        nodes[node_count - 2] = parent;

        node_count--;
        qsort(nodes, node_count, sizeof(huffman_node*), compare_nodes);
    }

    return nodes[0];
}

void encode_huffman(const char* str, code_map* map, int map_size, char** encoded_str, size_t* encoded_len) {
    size_t buffer_size = 1024 * 1024; 
    *encoded_str = (char*)malloc(buffer_size);
    if (*encoded_str == NULL) {
        perror("Error asignando memoria para el buffer de codificación");
        exit(EXIT_FAILURE);
    }
    
    size_t pos = 0;
    *encoded_len = 0;
    while (*str) {
        int found = 0;
        for (int i = 0; i < map_size; i++) {
            if (*str == map[i].character) {
                size_t code_len = strlen(map[i].code);
                while (pos + code_len >= buffer_size) {
                    buffer_size *= 2;
                    *encoded_str = (char*)realloc(*encoded_str, buffer_size);
                    if (*encoded_str == NULL) {
                        perror("Error reasignando memoria para el buffer de codificación");
                        exit(EXIT_FAILURE);
                    }
                }
                strcpy(*encoded_str + pos, map[i].code);
                pos += code_len;
                *encoded_len = pos;
                found = 1;
                break;
            }
        }
        if (!found) {
            fprintf(stderr, "Error: Carácter '%c' no encontrado en el mapa de códigos\n", *str);
            exit(EXIT_FAILURE);
        }
        str++;
    }
    (*encoded_str)[pos] = '\0';
}


void decode_huffman(const char* encoded_str, huffman_node* root, FILE* output_file) {
    huffman_node* current = root;
    for (int i = 0; encoded_str[i] != '\0'; i++) {
        if (encoded_str[i] == '0') {
            current = current->left;
        } else if (encoded_str[i] == '1') {
            current = current->right;
        }

        if (current->left == NULL && current->right == NULL) {
            fputc(current->character, output_file);
            current = root; 
        }
    }
}

void generate_codes(huffman_node* root, char* code, int length, code_map* map, int* map_size) {
    if (root == NULL) {
        return;
    }

    if (root->left == NULL && root->right == NULL) {
        map[*map_size].character = root->character;
        map[*map_size].code = (char*)malloc(length + 1);
        if (map[*map_size].code == NULL) {
            perror("Error asignando memoria para el código");
            exit(EXIT_FAILURE);
        }
        strncpy(map[*map_size].code, code, length);
        map[*map_size].code[length] = '\0';
        (*map_size)++;
    }

    if (root->left) {
        code[length] = '0';
        generate_codes(root->left, code, length + 1, map, map_size);
    }
    if (root->right) {
        code[length] = '1';
        generate_codes(root->right, code, length + 1, map, map_size);
    }
    code[length] = '\0'; 
}

void create_code_map(huffman_node* root, code_map* map, int* map_size) {
    char code[256]; 
    *map_size = 0;
    generate_codes(root, code, 0, map, map_size);
}

void free_huffman_tree(huffman_node* root) {
    if (root == NULL) {
        return;
    }

    free_huffman_tree(root->left);
    free_huffman_tree(root->right);

    free(root);
}

char* read_file(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        perror("Error abriendo el archivo");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        perror("Error asignando memoria");
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}

void write_binary_file(const char* file_path, const char* encoded_str) {
    FILE* file = fopen(file_path, "wb");
    if (!file) {
        perror("Error abriendo el archivo");
        exit(EXIT_FAILURE);
    }

    size_t length = strlen(encoded_str);
    unsigned char byte = 0;
    int bit_count = 0;

    for (size_t i = 0; i < length; ++i) {
        if (encoded_str[i] == '1') {
            byte |= (1 << (7 - bit_count));
        }

        bit_count++;
        if (bit_count == 8) {
            fwrite(&byte, sizeof(unsigned char), 1, file);
            byte = 0;
            bit_count = 0;
        }
    }

    if (bit_count > 0) {
        fwrite(&byte, sizeof(unsigned char), 1, file);
    }

    fclose(file);
}

char* read_binary_file(const char* file_path, size_t* length) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        perror("Error abriendo el archivo");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    *length = file_size * 8;
    char* buffer = (char*)malloc(*length + 1);
    if (!buffer) {
        perror("Error asignando memoria");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    unsigned char byte;
    size_t bit_index = 0;

    while (fread(&byte, sizeof(unsigned char), 1, file) == 1) {
        for (int bit_position = 7; bit_position >= 0; --bit_position) {
            buffer[bit_index++] = (byte & (1 << bit_position)) ? '1' : '0';
        }
    }

    buffer[*length] = '\0';

    fclose(file);
    return buffer;
}

void write_huffman_tree(FILE* file, huffman_node* root) {
    if (root == NULL) {
        fputc(0, file);
        return;
    }

    fputc(1, file); 

    unsigned char char_byte = root->character;
    for (int bit = 7; bit >= 0; bit--) {
        fputc((char_byte & (1 << bit)) ? '1' : '0', file);
    }

    unsigned char freq_byte[sizeof(int) * 8] = {0};
    for (int i = 0; i < sizeof(int) * 8; i++) {
        freq_byte[i] = (root->frequency & (1 << (sizeof(int) * 8 - 1 - i))) ? '1' : '0';
    }
    for (int i = 0; i < sizeof(int) * 8; i++) {
        fputc(freq_byte[i], file);
    }

    write_huffman_tree(file, root->left);
    write_huffman_tree(file, root->right);
}


huffman_node* read_huffman_tree(FILE* file) {
    int is_non_null = fgetc(file);
    if (is_non_null == 0) {
        return NULL;
    }

    huffman_node* node = (huffman_node*)malloc(sizeof(huffman_node));
    if (node == NULL) {
        perror("Error allocating memory for node");
        exit(EXIT_FAILURE);
    }

    unsigned char char_byte = 0;
    for (int bit = 7; bit >= 0; bit--) {
        int bit_value = fgetc(file) == '1' ? 1 : 0;
        char_byte |= (bit_value << bit);
    }
    node->character = char_byte;

    node->frequency = 0;
    for (int i = 0; i < sizeof(int) * 8; i++) {
        int bit_value = fgetc(file) == '1' ? 1 : 0;
        node->frequency |= (bit_value << (sizeof(int) * 8 - 1 - i));
    }

    node->left = read_huffman_tree(file);
    node->right = read_huffman_tree(file);
    
    return node;
}
void write_bits(FILE* file, const char* bits, size_t len) {
    unsigned char byte = 0;
    int bit_count = 0;

    for (size_t i = 0; i < len; i++) {
        if (bits[i] == '1') {
            byte |= (1 << (7 - bit_count));
        }
        bit_count++;

        if (bit_count == 8) {
            fwrite(&byte, sizeof(unsigned char), 1, file);
            byte = 0;
            bit_count = 0;
        }
    }

    if (bit_count > 0) {  
        fwrite(&byte, sizeof(unsigned char), 1, file);
    }
}

void process_directory(const char* dir_path, const char* compressed_file_path) {
    DIR* dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error abriendo el directorio");
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;
    FILE* compressed_file = fopen(compressed_file_path, "wb");
    if (!compressed_file) {
        perror("Error abriendo el archivo comprimido para escritura");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { 
            size_t path_len = strlen(dir_path) + strlen(entry->d_name) + 2; 
            char* input_file_path = (char*)malloc(path_len);
            if (input_file_path == NULL) {
                perror("Error asignando memoria");
                exit(EXIT_FAILURE);
            }
            snprintf(input_file_path, path_len, "%s/%s", dir_path, entry->d_name);

            char* input_str = read_file(input_file_path);

            int frequencies[256];
            char unique_characters[256];
            int unique_count;
            count_frequencies(input_str, frequencies, unique_characters, &unique_count);

            huffman_node* nodes[256];
            for (int i = 0; i < unique_count; i++) {
                nodes[i] = create_node(unique_characters[i], frequencies[i]);
            }

            huffman_node* root = build_huffman_tree(unique_count, nodes);

            code_map map[256];
            int map_size;
            create_code_map(root, map, &map_size);

            char* encoded_str;
            size_t encoded_len;
            encode_huffman(input_str, map, map_size, &encoded_str, &encoded_len);

            size_t name_len = strlen(entry->d_name);
            fwrite(&name_len, sizeof(size_t), 1, compressed_file);
            fwrite(entry->d_name, sizeof(char), name_len, compressed_file);

            write_huffman_tree(compressed_file, root);

            fwrite(&encoded_len, sizeof(size_t), 1, compressed_file);
            write_bits(compressed_file, encoded_str, encoded_len);

            for (int i = 0; i < map_size; i++) {
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
void read_bits(FILE* file, char* bits, size_t len) {
    unsigned char byte;
    int bit_count = 0;
    size_t bit_index = 0;

    while (bit_index < len) {
        if (bit_count == 0) {
            fread(&byte, sizeof(unsigned char), 1, file);
        }

        bits[bit_index] = ((byte >> (7 - bit_count)) & 1) ? '1' : '0';
        bit_count++;
        bit_index++;

        if (bit_count == 8) {
            bit_count = 0;
        }
    }
}

void decompress_files(const char* compressed_file_path, const char* output_dir) {
    FILE* compressed_file = fopen(compressed_file_path, "rb");
    if (!compressed_file) {
        perror("Error abriendo el archivo comprimido para lectura");
        exit(EXIT_FAILURE);
    }

    struct stat st = {0};
    if (stat(output_dir, &st) == -1) {
        mkdir(output_dir, 0700);
    }

    while (1) {
        size_t name_len;
        if (fread(&name_len, sizeof(size_t), 1, compressed_file) != 1) break;

        char* file_name = (char*)malloc(name_len + 1);
        if (file_name == NULL) {
            perror("Error asignando memoria");
            exit(EXIT_FAILURE);
        }
        fread(file_name, sizeof(char), name_len, compressed_file);
        file_name[name_len] = '\0';

        huffman_node* root = read_huffman_tree(compressed_file);

        size_t encoded_len;
        fread(&encoded_len, sizeof(size_t), 1, compressed_file);
        char* encoded_str = (char*)malloc(encoded_len + 1);
        if (encoded_str == NULL) {
            perror("Error asignando memoria");
            exit(EXIT_FAILURE);
        }
        encoded_str[encoded_len] = '\0';
        read_bits(compressed_file, encoded_str, encoded_len);

        size_t path_len = strlen(output_dir) + strlen(file_name) + 2; 
        char* output_file_path = (char*)malloc(path_len);
        if (output_file_path == NULL) {
            perror("Error asignando memoria");
            exit(EXIT_FAILURE);
        }
        snprintf(output_file_path, path_len, "%s/%s", output_dir, file_name);
        FILE* output_file = fopen(output_file_path, "wb");
        if (!output_file) {
            perror("Error abriendo el archivo de salida");
            exit(EXIT_FAILURE);
        }

        decode_huffman(encoded_str, root, output_file);
        fclose(output_file);

        free(file_name);
        free(encoded_str);
        free(output_file_path);
        free_huffman_tree(root);
    }

    fclose(compressed_file);
}

int main() {
    //const char* base_url = "https://www.gutenberg.org/browse/scores/top";
    //download_text_files(base_url);
    const char* input_dir = "books_to_compress";
    const char* compressed_file_path = "compressed_books/compressed.bin";
    const char* decompressed_dir = "decompressed_books";

    process_directory(input_dir, compressed_file_path);

    decompress_files(compressed_file_path, decompressed_dir);

    return 0;
}