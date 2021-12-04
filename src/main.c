#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dnwiki.h>

char *path = 0;
struct main_methods_t *root = 0;

int main(argsc, args, env)
int argsc;
char **args, **env;
{
    ma_add(&root, "download", download_main);
    ma_add(&root, "fetch", download_main);

    ma_add(&root, "reader", reader_main);
    ma_add(&root, "viewer", reader_main);

    ma_add(&root, "search", search_main);
    ma_add(&root, "find", search_main);

    ma_add(&root, "list", list_main);
    ma_add(&root, "iterate", list_main);

    ma_add(&root, "history", history_main);
    ma_add(&root, "log", history_main);

    int (*main_func)(int, char **) = 0;

    if ((main_func = ma_find(basename(*args)))
	&& !ma_find(basename(args[1]))) {
	return main_func(argsc, args);
    } else if ((main_func = ma_find(args[1]))) {
	return main_func(argsc + 1, args + 1);
    } else {
	ma_list_mains();
	return 1;
    }

    return 0;
}
