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
*/

#include "huffman_serial.h"

int compare_nodes_serial(const void* first_node, const void* second_node) {
    /*
        Función que compara dos nodos de Huffman en función de su frecuencia

        Entradas:
        - first_node: Puntero al primer nodo de Huffman
        - second_node: Puntero al segundo nodo de Huffman

        Salidas:
        - Diferencia entre las frecuencias de los nodos
    */
    huffman_node* node_a = *(huffman_node**)first_node;
    huffman_node* node_b = *(huffman_node**)second_node;
    return node_a->frequency - node_b->frequency;
}

huffman_node* create_node_serial(char character, int frequency) {
    /*
        Función que crea un nuevo nodo de Huffman

        Entradas:
        - character: Carácter del nodo
        - frequency: Frecuencia del carácter en el texto

        Salidas:
        - Puntero al nuevo nodo de Huffman
    */
    huffman_node* node = (huffman_node*)malloc(sizeof(huffman_node));
    node->character = character;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void count_frequencies_serial(const char* string, int* frequencies, char* unique_characters, int* unique_count) {
    /*
        Función que cuenta las frecuencias de los caracteres en una cadena de texto

        Entradas:
        - string: Cadena de texto
        - frequencies: Array para almacenar las frecuencias de cada carácter
        - unique_characters: Array para almacenar los caracteres únicos encontrados
        - unique_count: Puntero para almacenar el número de caracteres únicos encontrados

        Salidas:
        - Ninguna

        Restricciones:
        - La variable string no debe ser NULL
        - La variable frequencies no debe ser NULL
        - La variable unique_characters no debe ser NULL
        - La variable unique_count no debe ser NULL
    */
    int count[256] = {0};
    const char* pointer = string;

    while (*pointer) {
        count[(unsigned char)*pointer]++;
        pointer++;
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

huffman_node* build_huffman_tree_serial(int node_count, huffman_node** nodes) {
    /*
        Función que construye un árbol de Huffman a partir de una lista de nodos

        Entradas:
        - node_count: Número de nodos en la lista
        - nodes: Array de punteros a nodos de Huffman

        Salidas:
        - Puntero a la raíz del árbol de Huffman

        Restricciones:
        - La variable nodes no debe ser NULL
    */
    qsort(nodes, node_count, sizeof(huffman_node*), compare_nodes_serial);

    while (node_count > 1) {
        huffman_node* left = nodes[0];
        huffman_node* right = nodes[1];

        huffman_node* parent = create_node_serial('-', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;

        for (int i = 2; i < node_count; i++) {
            nodes[i - 2] = nodes[i];
        }
        nodes[node_count - 2] = parent;

        node_count--;
        qsort(nodes, node_count, sizeof(huffman_node*), compare_nodes_serial);
    }

    return nodes[0];
}

void encode_huffman_serial(const char* string_to_encode, code_map* map, int map_size, char** encoded_str, size_t* encoded_len) {
    /*
        Función que codifica una cadena de texto usando un mapa de códigos de Huffman

        Entradas:
        - string_to_encode: Cadena de texto a codificar
        - map: Mapa de códigos de Huffman
        - map_size: Tamaño del mapa de códigos
        - encoded_str: Doble puntero para almacenar la cadena de texto codificada
        - encoded_len: Puntero para almacenar la longitud de la cadena codificada

        Salidas:
        - Ninguna

        Restricciones:
        - La variable string_to_encode no debe ser NULL
        - La variable map no debe ser NULL
        - La variable encoded_str no debe ser NULL
        - La variable encoded_len no debe ser NULL
    */
    size_t buffer_size = 1024 * 1024; 
    *encoded_str = (char*)malloc(buffer_size);
    if (*encoded_str == NULL) {
        perror("Error asignando memoria para el buffer de codificación");
        exit(EXIT_FAILURE);
    }

    size_t pos = 0;
    *encoded_len = 0;
    while (*string_to_encode) {
        int found = 0;
        for (int i = 0; i < map_size; i++) {
            if (*string_to_encode == map[i].character) {
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
            fprintf(stderr, "Error: Carácter '%c' no encontrado en el mapa de códigos\n", *string_to_encode);
            exit(EXIT_FAILURE);
        }
        string_to_encode++;
    }
    (*encoded_str)[pos] = '\0';
}

void decode_huffman_serial(const char* encoded_str, huffman_node* root, FILE* output_file) {
    /*
        Función que decodifica una cadena codificada usando un árbol de Huffman

        Entradas:
        - encoded_str: Cadena de texto codificada
        - root: Raíz del árbol de Huffman
        - output_file: Archivo donde se escribirá la cadena decodificada

        Salidas:
        - Ninguna

        Restricciones:
        - La variable encoded_str no debe ser NULL
        - La variable root no debe ser NULL
        - La variable output_file no debe ser NULL
    */
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

void generate_codes_serial(huffman_node* root, char* code, int length, code_map* map, int* map_size) {
    /*
        Función que genera códigos de Huffman para cada carácter en el árbol

        Entradas:
        - root: Raíz del árbol de Huffman
        - code: Buffer para almacenar el código actual
        - length: Longitud del código actual
        - map: Mapa de códigos de Huffman a completar
        - map_size: Puntero para almacenar el tamaño del mapa

        Salidas:
        - Ninguna

        Restricciones:
        - La variable root no debe ser NULL
        - La variable map no debe ser NULL
        - La variable map_size no debe ser NULL
    */
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
        generate_codes_serial(root->left, code, length + 1, map, map_size);
    }
    if (root->right) {
        code[length] = '1';
        generate_codes_serial(root->right, code, length + 1, map, map_size);
    }
    code[length] = '\0'; 
}

void create_code_map_serial(huffman_node* root, code_map* map, int* map_size) {
    /*
        Función que crea un mapa de códigos de Huffman a partir de un árbol

        Entradas:
        - root: Raíz del árbol de Huffman
        - map: Mapa de códigos de Huffman a completar
        - map_size: Puntero para almacenar el tamaño del mapa

        Salidas:
        - Ninguna

        Restricciones:
        - La variable root no debe ser NULL
        - La variable map no debe ser NULL
        - La variable map_size no debe ser NULL
    */
    char code[256]; 
    *map_size = 0;
    generate_codes_serial(root, code, 0, map, map_size);
}

void free_huffman_tree_serial(huffman_node* root) {
    /*
        Función que libera la memoria utilizada por un árbol de Huffman

        Entradas:
        - root: Raíz del árbol de Huffman a liberar

        Salidas:
        - Ninguna

        Restricciones:
        - La variable root no debe ser NULL
    */
    if (root == NULL) {
        return;
    }

    free_huffman_tree_serial(root->left);
    free_huffman_tree_serial(root->right);

    free(root);
}

char* read_file_serial(const char* file_path) {
    /*
        Función que lee el contenido de un archivo en una cadena de texto

        Entradas:
        - file_path: Ruta del archivo a leer

        Salidas:
        - Cadena de texto que contiene el contenido del archivo

        Restricciones:
        - La variable file_path no debe ser NULL
    */
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

void write_huffman_tree_serial(FILE* file, huffman_node* root) {
    /*
        Función que escribe un árbol de Huffman en un archivo

        Entradas:
        - file: Archivo donde se escribirá el árbol
        - root: Raíz del árbol de Huffman a escribir

        Salidas:
        - Ninguna

        Restricciones:
        - La variable file no debe ser NULL
        - La variable root no debe ser NULL
    */
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

    write_huffman_tree_serial(file, root->left);
    write_huffman_tree_serial(file, root->right);
}

huffman_node* read_huffman_tree_serial(FILE* file) {
    /*
        Función que lee un árbol de Huffman desde un archivo

        Entradas:
        - file: puntero al archivo desde donde se leerá el árbol de Huffman

        Salidas:
        - Puntero al nodo raíz del árbol de Huffman

        Restricciones:
        - La variable file no debe ser NULL
    */
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

    node->left = read_huffman_tree_serial(file);
    node->right = read_huffman_tree_serial(file);

    return node;
}

void write_bits_serial(FILE* file, const char* bits, size_t len) {
    /*
        Función que escribe una cadena de bits en un archivo

        Entradas:
        - file: puntero al archivo donde se escribirán los bits
        - bits: cadena de bits a escribir
        - len: longitud de la cadena de bits

        Salidas:
        - Ninguna

        Restricciones:
        - La variable file no debe ser NULL
        - La variable bits no debe ser NULL
    */
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

void compress_files_serial(const char* dir_path, const char* compressed_file_path) {
    /*
        Función que comprime todos los archivos en un directorio y los guarda en un archivo comprimido

        Entradas:
        - dir_path: ruta del directorio que contiene los archivos a comprimir
        - compressed_file_path: ruta del archivo comprimido de salida

        Salidas:
        - Ninguna

        Restricciones:
        - La variable dir_path no debe ser NULL
        - La variable compressed_file_path no debe ser NULL
    */
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

            char* input_str = read_file_serial(input_file_path);

            int frequencies[256];
            char unique_characters[256];
            int unique_count;
            count_frequencies_serial(input_str, frequencies, unique_characters, &unique_count);

            huffman_node* nodes[256];
            for (int i = 0; i < unique_count; i++) {
                nodes[i] = create_node_serial(unique_characters[i], frequencies[i]);
            }

            huffman_node* root = build_huffman_tree_serial(unique_count, nodes);

            code_map map[256];
            int map_size;
            create_code_map_serial(root, map, &map_size);

            char* encoded_str;
            size_t encoded_len;
            encode_huffman_serial(input_str, map, map_size, &encoded_str, &encoded_len);

            size_t name_len = strlen(entry->d_name);
            fwrite(&name_len, sizeof(size_t), 1, compressed_file);
            fwrite(entry->d_name, sizeof(char), name_len, compressed_file);

            write_huffman_tree_serial(compressed_file, root);

            fwrite(&encoded_len, sizeof(size_t), 1, compressed_file);
            write_bits_serial(compressed_file, encoded_str, encoded_len);

            for (int i = 0; i < map_size; i++) {
                free(map[i].code);
            }
            free(input_str);
            free(encoded_str);
            free_huffman_tree_serial(root);
            free(input_file_path);
        }
    }

    closedir(dir);
    fclose(compressed_file);
}

void read_bits_serial(FILE* file, char* bits, size_t len) {
    /*
        Función que lee una cadena de bits de un archivo

        Entradas:
        - file: puntero al archivo desde donde se leerán los bits
        - bits: cadena para almacenar los bits leídos
        - len: longitud de la cadena de bits a leer

        Salidas:
        - Ninguna

        Restricciones:
        - La variable file no debe ser NULL
        - La variable bits no debe ser NULL
    */
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

void decompress_files_serial(const char* compressed_file_path, const char* output_dir) {
    /*
        Función que descomprime todos los archivos desde un archivo comprimido y guarda los resultados en un directorio

        Entradas:
        - compressed_file_path: ruta del archivo comprimido
        - output_dir: ruta del directorio donde se guardarán los archivos descomprimidos

        Salidas:
        - Ninguna

        Restricciones:
        - La variable compressed_file_path no debe ser NULL
        - La variable output_dir no debe ser NULL
    */
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

        huffman_node* root = read_huffman_tree_serial(compressed_file);

        size_t encoded_len;
        fread(&encoded_len, sizeof(size_t), 1, compressed_file);
        char* encoded_str = (char*)malloc(encoded_len + 1);
        if (encoded_str == NULL) {
            perror("Error asignando memoria");
            exit(EXIT_FAILURE);
        }
        encoded_str[encoded_len] = '\0';
        read_bits_serial(compressed_file, encoded_str, encoded_len);

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

        decode_huffman_serial(encoded_str, root, output_file);
        fclose(output_file);

        free(file_name);
        free(encoded_str);
        free(output_file_path);
        free_huffman_tree_serial(root);
    }

    fclose(compressed_file);
}