#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libavformat/avio.h>
#include <dnwiki.h>
#include <getopt.h>

struct option dm_opts[] = {
    { "help", 1, 0, 'h' },
    { "language_code", 1, 0, 'l' },
    { "wiki_type", 1, 0, 't' },
    { "wiki_title", 1, 0, 's' },
    { "wiki_title", 1, 0, 'T' },	// For legacy title.
    { "verbose", 0, 0, 'v' },
    { "retry_for", 1, 0, 'r' },	// Times of retry to read.
    {.name = 0 },
};

char *title = 0;

void dm_help();

int download_main(argsc, args, env)
int argsc;
char **args, **env;
{
    if (!args[1]) {
	dm_help();
	return 1;
    }
    return 0;
}
