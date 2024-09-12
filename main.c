#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "web_crawler.h"
#include "huffman_fork.h"


int main() {
    
    const char *input_dir = "books_to_compress";
    const char *compressed_file_path = "compressed_books/compressed.bin";
    const char *decompressed_dir = "decompressed_books";
    /*
    int n = 2;
    clock_t start, end;
    double totalTimeCompress = 0.0, totalTimeDecompress = 0.0;
    
    printf("Execution time for compression and decompression of text files\n");
    printf("Standard Implementation\n");
    printf("Number of iterations: %d\n", n);
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        start = clock();
        process_directory(input_dir, compressed_file_path);
        end = clock();
        totalTimeCompress += ((double)(end - start)) / CLOCKS_PER_SEC;
    }

    double averageCompressTime = totalTimeCompress / n;
    printf("Total time taken to compress: %f seconds\n", totalTimeCompress);
    printf("Average time taken to compress: %f seconds\n\n", averageCompressTime);

    for (int i = 0; i < n; i++) {
        start = clock();
        decompress_files(compressed_file_path, decompressed_dir);
        end = clock();
        totalTimeDecompress += ((double)(end - start)) / CLOCKS_PER_SEC;
    }

    double averageDecompressTime = totalTimeDecompress / n;
    printf("Total time taken to decompress: %f seconds\n", totalTimeDecompress);
    printf("Average time taken to decompress: %f seconds\n", averageDecompressTime);
*/
    process_directory(input_dir, compressed_file_path);
    decompress_files(compressed_file_path, decompressed_dir);
    return 0;
}