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
    //download_text_files(base_url);
    const char *input_dir = "books_to_compress";
    const char *compressed_file_path = "compressed_books/compressed.bin";
    const char *decompressed_dir = "decompressed_books";

    process_directory(input_dir, compressed_file_path);
    decompress_files(compressed_file_path, decompressed_dir);
    return 0;
}