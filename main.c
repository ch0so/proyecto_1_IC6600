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
    const char* base_url = "https://www.gutenberg.org/browse/scores/top";
    download_text_files(base_url);
    const char *input_dir = "books_to_compress";
    const char *compressed_file_dir = "compressed_books";
    const char *compressed_file_path = "compressed_books/compressed.bin";
    const char *decompressed_dir = "decompressed_books";

    struct stat st = {0};

    if (stat(compressed_file_dir, &st) == -1) {
        mkdir(compressed_file_dir, 0700);
    }

    if (stat(decompressed_dir, &st) == -1) {
        mkdir(decompressed_dir, 0700);
    }

    //process_directory(input_dir, compressed_file_path);
    //decompress_files(compressed_file_path, decompressed_dir);
    return 0;
}