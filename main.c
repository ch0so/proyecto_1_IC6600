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
    //const char* base_url = "https://www.gutenberg.org/browse/scores/top";
    const char *input_dir = "books_to_compress";
    const char *compressed_file_dir = "compressed_books";
    const char *compressed_file_path = "compressed_books/compressed.bin";
    const char *decompressed_dir = "decompressed_books";


    //download_text_files(base_url);
    struct stat dir_stat = {0};

    if (stat(compressed_file_dir, &dir_stat) == -1) {
        mkdir(compressed_file_dir, 0700);
    }

    if (stat(compressed_file_dir, &dir_stat) != -1) {
        remove_directory(compressed_file_dir);
    }

    mkdir(compressed_file_dir, 0700);

    if (stat(decompressed_dir, &dir_stat) != -1) {
        remove_directory(decompressed_dir);
    }

    mkdir(decompressed_dir, 0700);

    compress_files_fork(input_dir, compressed_file_path);
    decompress_files_fork(compressed_file_path, decompressed_dir);

    return 0;
}