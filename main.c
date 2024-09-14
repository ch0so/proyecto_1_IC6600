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
    - GeeksforGeeks. (2024, enero 09). Command Line Arguments in C [Argumentos de línea de comandos en C]. GeeksforGeeks. https://www.geeksforgeeks.org/command-line-arguments-in-c-cpp/
*/
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "web_crawler.h"
#include "huffman_fork.h"
#include "huffman_serial.h"
#include "huffman_threads.h"

int main(int argc, char *argv[]) {
    /*
        Función principal que maneja las opciones del programa

        Entradas:
        - argc: número de argumentos de línea de comandos
        - argv: array de cadenas que contiene los argumentos de línea de comandos

        Salidas:
        - Ninguna

        Restricciones:
        - Debe haber exactamente un argumento además del nombre del programa
        - Los argumentos deben ser uno de los siguientes:
        - download_books
        - clear_directory_compressed
        - clear_directory_decompressed
        - compressed_fork
        - decompressed_fork
        - compressed_serial
        - decompressed_serial
        - compressed_threads
        - decompressed_threads
    */

    // Verificación de argumentos
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <opción>\n", argv[0]);
        fprintf(stderr, "Opciones disponibles:\n");
        fprintf(stderr, "  download_books\n");
        fprintf(stderr, "  clear_directory_compressed\n");
        fprintf(stderr, "  clear_directory_decompressed\n");
        fprintf(stderr, "  compressed_fork\n");
        fprintf(stderr, "  decompressed_fork\n");
        fprintf(stderr, "  compressed_serial\n");
        fprintf(stderr, "  decompressed_serial\n");
        fprintf(stderr, "  compressed_threads\n");
        fprintf(stderr, "  decompressed_threads\n");
        return EXIT_FAILURE;
    }

    struct stat dir_stat = {0};
    const char *input_dir = "books_to_compress";
    const char *compressed_file_dir = "compressed_books";
    const char *compressed_file_path = "compressed_books/compressed.bin";
    const char *decompressed_dir = "decompressed_books";
    struct timeval start_time, end_time;
    long seconds, microseconds, nanoseconds;

    // Manejo de la opción download_books
    if (strcmp(argv[1], "download_books") == 0) {
        const char* base_url = "https://www.gutenberg.org/browse/scores/top";
        download_text_files(base_url);
    } 
    // Manejo de la opción clear_directory_compressed
    else if (strcmp(argv[1], "clear_directory_compressed") == 0) {
        if (stat(compressed_file_dir, &dir_stat) != -1) {
            remove_directory(compressed_file_dir);
        }
        mkdir(compressed_file_dir, 0700);
    } 
    // Manejo de la opción clear_directory_decompressed
    else if (strcmp(argv[1], "clear_directory_decompressed") == 0) {
        if (stat(decompressed_dir, &dir_stat) != -1) {
            remove_directory(decompressed_dir);
        }
        mkdir(decompressed_dir, 0700);
    } 
    // Manejo de la opción compressed_fork
    else if (strcmp(argv[1], "compressed_fork") == 0) {
        if (stat(compressed_file_dir, &dir_stat) == -1) { 
            if (mkdir(compressed_file_dir, 0700) != 0) {  
                perror("Error creando el directorio");
            }
        }
        gettimeofday(&start_time, NULL);
        compress_files_fork(input_dir, compressed_file_path);
        gettimeofday(&end_time, NULL);
        seconds = end_time.tv_sec - start_time.tv_sec;
        microseconds = end_time.tv_usec - start_time.tv_usec;
        nanoseconds = (seconds * 1e9) + (microseconds * 1e3);
        printf("El programa tardó %ld nanosegundos en ejecutarse\n", nanoseconds);
    } 
    // Manejo de la opción decompressed_fork
    else if (strcmp(argv[1], "decompressed_fork") == 0) {
        if (stat(decompressed_dir, &dir_stat) == -1) { 
            if (mkdir(decompressed_dir, 0700) != 0) {  
                perror("Error creando el directorio");
            }
        }
        gettimeofday(&start_time, NULL);
        decompress_files_fork(compressed_file_path, decompressed_dir);
        gettimeofday(&end_time, NULL);
        seconds = end_time.tv_sec - start_time.tv_sec;
        microseconds = end_time.tv_usec - start_time.tv_usec;
        nanoseconds = (seconds * 1e9) + (microseconds * 1e3);
        printf("El programa tardó %ld nanosegundos en ejecutarse\n", nanoseconds);
    } 
    // Manejo de la opción compressed_serial
    else if (strcmp(argv[1], "compressed_serial") == 0) {
        if (stat(compressed_file_dir, &dir_stat) == -1) { 
            if (mkdir(compressed_file_dir, 0700) != 0) {  
                perror("Error creando el directorio");
            }
        }
        gettimeofday(&start_time, NULL);
        compress_files_serial(input_dir, compressed_file_path);
        gettimeofday(&end_time, NULL);
        seconds = end_time.tv_sec - start_time.tv_sec;
        microseconds = end_time.tv_usec - start_time.tv_usec;
        nanoseconds = (seconds * 1e9) + (microseconds * 1e3);
        printf("El programa tardó %ld nanosegundos en ejecutarse\n", nanoseconds);
    } 
    // Manejo de la opción decompressed_serial
    else if (strcmp(argv[1], "decompressed_serial") == 0) {
        if (stat(decompressed_dir, &dir_stat) == -1) { 
            if (mkdir(decompressed_dir, 0700) != 0) {  
                perror("Error creando el directorio");
            }
        }
        gettimeofday(&start_time, NULL);
        decompress_files_serial(compressed_file_path, decompressed_dir);
        gettimeofday(&end_time, NULL);
        seconds = end_time.tv_sec - start_time.tv_sec;
        microseconds = end_time.tv_usec - start_time.tv_usec;
        nanoseconds = (seconds * 1e9) + (microseconds * 1e3);
        printf("El programa tardó %ld nanosegundos en ejecutarse\n", nanoseconds);
    } 
    // Manejo de la opción compressed_threads
    else if (strcmp(argv[1], "compressed_threads") == 0) {
        if (stat(compressed_file_dir, &dir_stat) == -1) { 
            if (mkdir(compressed_file_dir, 0700) != 0) {  
                perror("Error creando el directorio");
            }
        }
        gettimeofday(&start_time, NULL);
        compress_files_threads(input_dir, compressed_file_path);
        gettimeofday(&end_time, NULL);
        seconds = end_time.tv_sec - start_time.tv_sec;
        microseconds = end_time.tv_usec - start_time.tv_usec;
        nanoseconds = (seconds * 1e9) + (microseconds * 1e3);
        printf("El programa tardó %ld nanosegundos en ejecutarse\n", nanoseconds);
    } 
    // Manejo de la opción decompressed_threads
    else if (strcmp(argv[1], "decompressed_threads") == 0) {
        if (stat(decompressed_dir, &dir_stat) == -1) { 
            if (mkdir(decompressed_dir, 0700) != 0) {  
                perror("Error creando el directorio");
            }
        }
        gettimeofday(&start_time, NULL);
        decompress_files_threads(compressed_file_path, decompressed_dir);
        gettimeofday(&end_time, NULL);
        seconds = end_time.tv_sec - start_time.tv_sec;
        microseconds = end_time.tv_usec - start_time.tv_usec;
        nanoseconds = (seconds * 1e9) + (microseconds * 1e3);
        printf("El programa tardó %ld nanosegundos en ejecutarse\n", nanoseconds);
    } 
    // Manejo de opción inválida
    else {
        fprintf(stderr, "Opción no válida: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}