#ifndef WEB_CRAWLER_H
#define WEB_CRAWLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <regex.h>
#include <tidy.h>
#include <tidybuffio.h>
#include <errno.h>

typedef struct {
    char *data;
    size_t size;
} memory_data;

size_t write_memory_callback(void *contents, size_t size, size_t number_of_members, void *user_pointer);

void extract_links_from_html(const char *html);

char *find_link(const char *html);

void extract_title_and_author(const char *filename, char **title, char **author);

void process_link(CURL *curl, const char *url);

#endif // WEB_CRAWLER_H