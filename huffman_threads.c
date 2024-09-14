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

#include "huffman_threads.h"

// Mutex para sincronizar el acceso a la cadena codificada
pthread_mutex_t encoded_str_mutex;

// Mutex para sincronizar el acceso al archivo comprimido
pthread_mutex_t file_mutex;

// Mutex para sincronizar el acceso a la cadena decodificada
pthread_mutex_t decode_str_mutex;

int compare_nodes_threads(const void* first_node, const void* second_node) {
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

huffman_node* create_node_threads(char character, int frequency) {
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

void count_frequencies_threads(const char* string, int* frequencies, char* unique_characters, int* unique_count) {
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

huffman_node* build_huffman_tree_threads(int node_count, huffman_node** nodes) {
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
    qsort(nodes, node_count, sizeof(huffman_node*), compare_nodes_threads);

    while (node_count > 1) {
        huffman_node* left = nodes[0];
        huffman_node* right = nodes[1];

        huffman_node* parent = create_node_threads('-', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;

        for (int i = 2; i < node_count; i++) {
            nodes[i - 2] = nodes[i];
        }
        nodes[node_count - 2] = parent;

        node_count--;
        qsort(nodes, node_count, sizeof(huffman_node*), compare_nodes_threads);
    }

    return nodes[0];
}

void *encode_huffman_threads_aux(void *arg) {
    /*
        Función auxiliar que codifica una cadena utilizando el algoritmo de Huffman en múltiples hilos

        Entradas:
        - arg: Puntero a una estructura huffman_encoded_data con los datos necesarios para la codificación (cadena, mapa de códigos, etc)

        Salidas:
        - Ninguna

        Restricciones:
        - La estructura huffman_encoded_data debe estar correctamente inicializada
        - La memoria para el string codificado debe ser asignada dinámicamente y liberada al finalizar
    */
    huffman_encoded_data *data = (huffman_encoded_data *)arg;

    const char *str = data->str + data->initial;
    code_map *map = data->map;
    int map_size = data->map_size;

    size_t buffer_size = 1024;
    char *thread_encoded_str = (char *)malloc(buffer_size);

    size_t pos = 0;

    for (long i = data->initial; i < data->final; i++, str++) {
        int found = 0;
        for (int j = 0; j < map_size; j++) {
            if (*str == map[j].character) {
                size_t code_len = strlen(map[j].code);
                while (pos + code_len >= buffer_size) {
                    buffer_size *= 2;
                    thread_encoded_str = (char *)realloc(thread_encoded_str, buffer_size);
                    if (!thread_encoded_str) {
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
        if (!found) {
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

void encode_huffman_threads(const char *str, code_map *map, int map_size, char **encoded_str, size_t *encoded_len) {
    /*
        Función que divide la cadena a codificar y utiliza hilos para paralelizar el proceso de codificación de Huffman

        Entradas:
        - str: Cadena original a codificar
        - map: Mapa de códigos de Huffman a utilizar
        - map_size: Tamaño del mapa de códigos
        - encoded_str: Puntero para almacenar la cadena codificada resultante
        - encoded_len: Puntero para almacenar la longitud de la cadena codificada

        Salidas:
        - encoded_str: Cadena codificada
        - encoded_len: Longitud de la cadena codificada

        Restricciones:
        - La variable str no debe ser NULL
        - El mapa de códigos debe estar correctamente inicializado y tener al menos un código válido
    */
    long string_length = strlen(str);
    long segment_length = string_length / ENCODED_DATA_THREADS;
    

    pthread_t threads[ENCODED_DATA_THREADS];
    huffman_encoded_data thread_data[ENCODED_DATA_THREADS];

    pthread_mutex_init(&encoded_str_mutex, NULL);
    *encoded_len = 0;

    for (int i = 0; i < ENCODED_DATA_THREADS; i++) {
        thread_data[i].str = str;
        thread_data[i].map = map;
        thread_data[i].map_size = map_size;
        thread_data[i].initial = i * segment_length;
        thread_data[i].final = (i == ENCODED_DATA_THREADS - 1) ? string_length : (i + 1) * segment_length;

        pthread_create(&threads[i], NULL, encode_huffman_threads_aux, (void *)&thread_data[i]);
    }

    for (int i = 0; i < ENCODED_DATA_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    size_t total_size = 0;
    for (int i = 0; i < ENCODED_DATA_THREADS; i++) {
        total_size += strlen(thread_data[i].encoded_str);
    }

    *encoded_str = (char *)malloc(total_size + 1);  
    (*encoded_str)[0] = '\0';

    for (int i = 0; i < ENCODED_DATA_THREADS; i++) {
        strcat(*encoded_str, thread_data[i].encoded_str);
        *encoded_len += strlen(thread_data[i].encoded_str);
        free(thread_data[i].encoded_str);
    }

    pthread_mutex_destroy(&encoded_str_mutex);
}

void decode_huffman_threads(const char* encoded_str, huffman_node* root, FILE* output_file) {
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

void generate_codes_threads(huffman_node* root, char* code, int length, code_map* map, int* map_size) {
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
        generate_codes_threads(root->left, code, length + 1, map, map_size);
    }
    if (root->right) {
        code[length] = '1';
        generate_codes_threads(root->right, code, length + 1, map, map_size);
    }
    code[length] = '\0'; 
}

void create_code_map_threads(huffman_node* root, code_map* map, int* map_size) {
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
    generate_codes_threads(root, code, 0, map, map_size);
}

void free_huffman_tree_threads(huffman_node* root) {
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

    free_huffman_tree_threads(root->left);
    free_huffman_tree_threads(root->right);

    free(root);
}

char* read_file_threads(const char* file_path) {
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

void write_huffman_tree_threads(FILE* file, huffman_node* root) {
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

    write_huffman_tree_threads(file, root->left);
    write_huffman_tree_threads(file, root->right);
}

huffman_node* read_huffman_tree_threads(FILE* file) {
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

    node->left = read_huffman_tree_threads(file);
    node->right = read_huffman_tree_threads(file);

    return node;
}

void write_bits_threads(FILE* file, const char* bits, size_t len) {
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

void *thread_process(void *arg) {
    /*
        Función auxiliar que procesa un archivo individual utilizando múltiples hilos para la compresión con Huffman

        Entradas:
        - arg: Puntero a una estructura args_thread con los datos necesarios para procesar el archivo

        Salidas:
        - Ninguna

        Restricciones:
        - La estructura args_thread debe estar correctamente inicializada
        - La función debe ejecutarse dentro de un hilo y debe haber mutua exclusión para escribir en el archivo comprimido
    */
    args_thread *args = (args_thread *)arg;

    char *input_str = read_file_threads(args->file_path);
    int frequencies[256];
    char unique_characters[256];
    int unique_count;
    count_frequencies_threads(input_str, frequencies, unique_characters, &unique_count);

    huffman_node *nodes[256];
    for (int i = 0; i < unique_count; i++) {
        nodes[i] = create_node_threads(unique_characters[i], frequencies[i]);
    }

    huffman_node *root = build_huffman_tree_threads(unique_count, nodes);

    code_map map[256];
    int map_size;
    create_code_map_threads(root, map, &map_size);

    char *encoded_str;
    size_t encoded_len;
    encode_huffman_threads(input_str, map, map_size, &encoded_str, &encoded_len);

    pthread_mutex_lock(&file_mutex);
    size_t name_len = strlen(args->file_name);
    fwrite(&name_len, sizeof(size_t), 1, args->compressed_file); 
    fwrite(args->file_name, sizeof(char), name_len, args->compressed_file); 

    write_huffman_tree_threads(args->compressed_file, root); 
    fwrite(&encoded_len, sizeof(size_t), 1, args->compressed_file); 
    write_bits_threads(args->compressed_file, encoded_str, encoded_len);
    pthread_mutex_unlock(&file_mutex);

    for (int i = 0; i < map_size; i++) {
        free(map[i].code);
    }

    free(input_str);
    free(encoded_str);
    free_huffman_tree_threads(root);

    free(args);

    return NULL;
};

void compress_files_threads(const char *dir_path, const char *compressed_file_path) {
    /*
        Función que comprime múltiples archivos de un directorio utilizando hilos para paralelizar la compresión con Huffman

        Entradas:
        - dir_path: Ruta al directorio que contiene los archivos a comprimir
        - compressed_file_path: Ruta del archivo comprimido de salida

        Salidas:
        - Ninguna

        Restricciones:
        - El directorio especificado en dir_path debe existir y contener archivos regulares
        - El archivo comprimido de salida debe poder abrirse para escritura
    */
    pthread_mutex_init(&file_mutex, NULL);
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error abriendo el directorio");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    FILE *compressed_file = fopen(compressed_file_path, "wb");
    if (!compressed_file) {
        perror("Error abriendo el archivo comprimido para escritura");
        exit(EXIT_FAILURE);
    }

    int amount_files;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            amount_files++;
        }
    }
    closedir(dir);

    pthread_t threads[amount_files];

    dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error abriendo el directorio");
        exit(EXIT_FAILURE);
    }

    int thread_counter = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            args_thread *args = (args_thread *)malloc(sizeof(args_thread));
            if (args == NULL) {
                printf("Error assigning memory to thread arg");
                exit(EXIT_FAILURE);
            }

            snprintf(args->file_path, sizeof(args->file_path), "%s/%s", dir_path, entry->d_name);
            args->thread_id = thread_counter;
            strncpy(args->file_name, entry->d_name, sizeof(args->file_name) - 1);
            args->compressed_file = compressed_file;

            pthread_create(&threads[thread_counter], NULL, thread_process, args);

            thread_counter++;
        }
    }

    for (int i = 0; i < thread_counter; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&file_mutex); 

    closedir(dir);
    fclose(compressed_file);
}

void read_bits_threads(FILE* file, char* bits, size_t len) {
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

void* decompress_file_threads(void* args) {
    /*
        Función que descomprime un archivo comprimido y guarda el contenido en un archivo de salida

        Entradas:
        - compressed_file_path: ruta del archivo comprimido
        - file_name: nombre del archivo de salida
        - output_file_path: ruta del archivo de salida
        - position: posición en el archivo comprimido desde donde comenzar la descompresión
        - huffman_tree: árbol de Huffman para la descompresión
        - encoded_str: cadena codificada para la descompresión

        Salidas:
        - Ninguna

        Restricciones:
        - La variable compressed_file_path no debe ser NULL
        - La variable file_name no debe ser NULL
        - La variable output_file_path no debe ser NULL
        - El árbol de Huffman no debe ser NULL
        - La cadena codificada no debe ser NULL
    */
    decompress_args_t* decompress_args = (decompress_args_t*)args;
    FILE* output_file = fopen(decompress_args->output_file_path, "wb");
    if (!output_file) {
        perror("Error abriendo el archivo de salida");
        pthread_exit(NULL);
    }

    FILE* compressed_file = fopen(decompress_args->compressed_file_path, "rb");
    if (!compressed_file) {
        perror("Error abriendo el archivo comprimido para lectura");
        fclose(output_file);
        pthread_exit(NULL);
    }
    fseek(compressed_file, decompress_args->position, SEEK_SET);

    decode_huffman_threads(decompress_args->encoded_str, decompress_args->huffman_tree, output_file);

    fclose(output_file);
    fclose(compressed_file);
    pthread_exit(NULL);
}

void decompress_files_threads(const char* compressed_file_path, const char* output_dir) {
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

    char** output_file_paths = (char**)malloc(INITIAL_CAPACITY * sizeof(char*));
    if (!output_file_paths) {
        perror("Error asignando memoria para las posiciones de los archivos");
        exit(EXIT_FAILURE);
    }

    char** output_files = (char**)malloc(INITIAL_CAPACITY * sizeof(char*));
    if (!output_files) {
        perror("Error asignando memoria para los nombres de archivos");
        exit(EXIT_FAILURE);
    }

    long long int* output_integers = (long long int*)malloc(INITIAL_CAPACITY * sizeof(long long int));
    if (!output_integers) {
        perror("Error asignando memoria para los enteros largos");
        exit(EXIT_FAILURE);
    }

    huffman_node** huffman_trees = (huffman_node**)malloc(INITIAL_CAPACITY * sizeof(huffman_node*));
    if (!huffman_trees) {
        perror("Error asignando memoria para los árboles de Huffman");
        exit(EXIT_FAILURE);
    }

    char** encoded_strings = (char**)malloc(INITIAL_CAPACITY * sizeof(char*));
    if (!encoded_strings) {
        perror("Error asignando memoria para las cadenas codificadas");
        exit(EXIT_FAILURE);
    }

    size_t output_file_count = 0;
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
        output_files[output_file_count] = file_name;
        output_integers[output_file_count] = ftell(compressed_file);

        huffman_node* root = read_huffman_tree_threads(compressed_file);
        size_t encoded_len;
        fread(&encoded_len, sizeof(size_t), 1, compressed_file);

        char* encoded_str = (char*)malloc(encoded_len + 1);
        if (encoded_str == NULL) {
            perror("Error asignando memoria");
            exit(EXIT_FAILURE);
        }
        encoded_str[encoded_len] = '\0';
        read_bits_threads(compressed_file, encoded_str, encoded_len);

        huffman_trees[output_file_count] = root;
        encoded_strings[output_file_count] = encoded_str;

        size_t path_len = strlen(output_dir) + strlen(file_name) + 2;
        char* output_file_path = (char*)malloc(path_len);
        if (output_file_path == NULL) {
            perror("Error asignando memoria");
            exit(EXIT_FAILURE);
        }
        snprintf(output_file_path, path_len, "%s/%s", output_dir, file_name);
        output_file_paths[output_file_count] = output_file_path;
        output_file_count++;
    }
    fclose(compressed_file);

    pthread_t* threads = (pthread_t*)malloc(output_file_count * sizeof(pthread_t));
    if (!threads) {
        perror("Error asignando memoria para los hilos");
        exit(EXIT_FAILURE);
    }

    decompress_args_t* decompress_args = (decompress_args_t*)malloc(output_file_count * sizeof(decompress_args_t));
    if (!decompress_args) {
        perror("Error asignando memoria para los argumentos de los hilos");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < output_file_count; ++i) {
        decompress_args[i].compressed_file_path = compressed_file_path;
        decompress_args[i].file_name = output_files[i];
        decompress_args[i].output_file_path = output_file_paths[i];
        decompress_args[i].position = output_integers[i];
        decompress_args[i].huffman_tree = huffman_trees[i];
        decompress_args[i].encoded_str = encoded_strings[i];

        if (pthread_create(&threads[i], NULL, decompress_file_threads, &decompress_args[i]) != 0) {
            perror("Error al crear el hilo");
            exit(EXIT_FAILURE);
        }
    }

    for (size_t i = 0; i < output_file_count; ++i) {
        pthread_join(threads[i], NULL);
    }

    for (size_t i = 0; i < output_file_count; ++i) {
        free(output_files[i]);
        free(output_file_paths[i]);
        free(encoded_strings[i]);
        free_huffman_tree_threads(huffman_trees[i]);
    }

    free(output_files);
    free(output_file_paths);
    free(output_integers);
    free(huffman_trees);
    free(encoded_strings);
    free(threads);
    free(decompress_args);
}