#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef DNWIKI_INC
#define DNWIKI_INC
struct main_methods_t {
    char *name;
    int (*main)(int, char **);
    struct main_methods_t *next;
};

extern char *path;
extern struct main_methods_t *root;

void ma_add(struct main_methods_t **r, char *name, int (*m)(int, char **));
typedef int (*ma_func_t)(int, char **);

ma_func_t ma_find(char *name);

int download_main(int, char **);
int reader_main(int, char **);
int search_main(int, char **);
int list_main(int, char **);
int history_main(int, char **);
#endif
