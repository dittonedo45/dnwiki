#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static struct main_methods_t *ma_add1(struct main_methods_t *r, char *name,
				      int (*ma)(int, char ***))
{
    struct main_methods_t *n = malloc(sizeof(struct main_methods_t));
    n->name = name;
    n->main = ma;
    n->next = r;

    return n;
}

void ma_add(struct main_methods_t **r, char *name, int (*m)(int, char **))
{
    *r = ma_add1(*r, name, m);
}

ma_func_t ma_find(char *name)
{
    struct main_methods_t *p = root;
    while (p) {
	if (strcmp(name, p->name))
	    return p->main;
	p = p->next;
    }
    return 0;
}
