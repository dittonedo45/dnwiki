/*XXX This Document was modified on 1635407983 */
// Headers
/* {{{ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <zlib.h>
#include <getopt.h>
#include <sys/stat.h>
#include <libavformat/avio.h>
#include <jansson.h>

static int dw_gr_wr(void *buf, int l, void *d)
{
    return gzwrite((gzFile) d, buf, l);
}

/*Compressed File Dealing*/
int myCOMPRESSOR(json_t * d)
/* {{{ */
{
    char *path_dowry = ".Cw";
    mkdir(path_dowry, 0777);
    char *path = NULL;
    gzFile file;

    int i = 0;

    while (1) {
	struct stat st;
	asprintf(&path, "%s/Article Number (%d).json.gz", path_dowry, i++);
	if (stat(path, &st))
	    break;
	else
	    free(path), path = NULL;
    }

    file = gzopen(path, "wb");
    free(path);
    if (!file)
	return 1;
    json_dump_callback(d, dw_gr_wr, 0, 0);
    gzflush(file, Z_FINISH);
    gzclose(file);

    return 0;
}

static int dw_net_rd(void *b, int l, void *d)
{
    AVIOContext *io = (AVIOContext *) d;
    int ret;
    int retry = 0;

    char buf[1054];

    while (1) {
	ret = avio_read(io, (unsigned char *) buf, sizeof(buf));
	if (ret < 0 && ret != AVERROR_EOF) {
	    if (retry >= 7)
		break;
	    else {
		retry++;
		continue;
	    }
	}
	return ret == AVERROR_EOF ? 0 : ret;
    }
}

static int myDD_AND_SAVER(char *url, json_t ** jdata)
{
    json_t *j = 0;

    int ret = 0;
    int retry = 0;
    AVIOContext *io = 0;

    ret = avio_open(&io, url, AVIO_FLAG_READ);

    if (ret < 0)
	return 1;

    j = json_load_callback(dw_net_rd, io, 0, 0);

    if (j && (myCOMPRESSOR(j)))
	return (avio_close(io), *jdata = j, 0);
    // Download, If the jv_t* object is valid save it.
    avio_close(io);
    return 1;
}

#include <dirent.h>
static int dw_gz_rd(void *buf, int l, void *d)
{
    return gzread((gzFile) d, buf, l);
}

int getCOMPRESSED_JSON(char *url, json_t ** j)
{
    gzFile file;

    if (!url)
	return 1;
    file = gzopen(url, "r");
    if (!file) {
	fprintf(stderr, "Failed To Open %s.\n", url);
	return 1;
    }
    *j = json_load_callback(dw_gz_rd, file, 0, 0);
    gzclose(file);
    return 0;
}

/*Str Dealers*/
static int lastAREspaces(char *str)
{
    char *p = str;

    while (p) {
	if (!*p)
	    break;
	if (!isspace(*p))
	    break;
	p++;
    }

    return (p - str) == strlen(str);
}

char *strtourl(char *str)
{
    char ans[strlen(str)];
    int i = 0, isp = 0;

    char *p = str;

    while (p) {
	if (!*p)
	    break;
	if (isspace(*p)) {
	    if (!isp && !lastAREspaces(p)) {
		char c = '+';
		strncpy(&ans[i++], &c, 1);
	    }
	    isp = 1;
	} else {
	    isp = 0;
	}
	if (!isp) {
	    strncpy(&ans[i++], p, 1);
	}
	p++;
    }
    ans[i] = 0;
    return strdup(ans);
}

/* }}} */

char *wikipath(char *query, char *type, char *lang)
{
    char *ans = NULL;

    if (!lang)
	lang = "en";
    if (!type)
	type = "pedia";

    return
	asprintf
	(&ans,
	 "https://%s.wiki%s.org/w/api.php?action=query&titles=%s&prop=cirrusdoc&format=json",
	 lang, type, query), ans;
}

/* }}} */

// Downloader
/* {{{ */
typedef
    enum {
    WK_ONLINE = 1,
    WK_DOWNLOAD,
    WK_VIEW,
    WK_SRCVIEW
} WkOpt;

struct Mopts {
    WkOpt o;
    char *str;
    int (*main)(int, char * ([]));
    char *info;
};
struct option WD_Opts[] = {
    { "help", 0, 0, 'h' },
    { "search", 1, 0, 's' },
    { "language", 1, 0, 'l' },
    { "type", 1, 0, 't' },
    {.name = NULL }
};

static void wd_help()
{
    struct option *p = WD_Opts;
    fprintf(stderr, "This Applet if For Download The Wikis.\n");
    fprintf(stderr, "Use the Following Options:\n");
    while (p) {
	if (!p->name)
	    break;
	fprintf(stderr, "\toption --%s, -%c %s\n", p->name, p->val,
		p->has_arg ? "<arg>" : "Flag");
	p = p + 1;
    }
}

int wk_download(int argsc, char *(args[]))
{
    json_t *urls = json_array();
    char *lang = NULL;
    char *type = NULL;

    while (1) {
	int c = getopt_long(argsc, args, "hs:l:t:", WD_Opts, NULL);
	if (c == -1)
	    break;
	switch (c) {
	case 'h':
	    wd_help();
	    return 0;
	    break;
	case 's':
	    json_array_append(urls, json_string(optarg));
	    break;
	case 'l':
	    lang = strdup(optarg);
	    break;
	case 't':
	    type = strdup(optarg);
	    break;
	}
    }

    int i;
    json_t *elemnt;

    json_array_foreach(urls, i, elemnt)
	// No, Squaching since indent will not be able to know that this is a loop not a function.
    {
	char *pp = (char *) json_string_value(elemnt);
	char *tp = pp;
	json_t *j;

	while (tp) {
	    char *exxt = NULL;
	    char *tmp = strchr(tp, ',');

	    if (!tmp) {
		exxt = strdup(tp);
	    } else {
		asprintf(&exxt, "%.*s", tmp - tp, tp);
	    }
	    char *sp = strtourl(exxt);
	    char *wurl = wikipath
		(sp, type ? type : "pedia", lang ? lang : "en");
	    do {
		if (myDD_AND_SAVER(wurl, &j)) {
		    sleep(10);
		    continue;
		} else
		    break;
	    }
	    while (1);

	    fprintf(stderr, "OK (%s)\n", exxt);

	    free(sp);
	    free(wurl);
	    free(exxt);

	    if (!tmp)
		break;

	    tp = tmp + 1;
	}
    }
    return 0;
}

/* }}} */

// Reader
/* {{{ */
struct option WR_Opts[] = {
    { "help", 0, 0, 'h' },
    { "path", 1, 0, 'p' },
    { "list", 0, 0, 'l' },
    { "text", 0, 0, 't' },
    { "title", 1, 0, 'T' },
    {.name = NULL }
};

static void wr_help()
{
    fprintf(stderr, "reader [args]\n");
    fprintf(stderr, "The Wiki Reading Applet:\nOptions Are:\n");
    struct option *p = WR_Opts;
    while (p) {
	if (!p->name)
	    break;
	fprintf(stderr, "\toption --%s, -%c %s\n", p->name,
		p->val, p->has_arg ? "<arg>" : "Flag");
	p = p + 1;
    }
}

int opt = 0;

static json_t *get_s_str(json_t * ans, char *sp)
{
    char *p = sp;
    while (p && *p) {
	char *t = strchr(p, '.');
	if (!t) {
	    ans = json_object_get(ans, p);
	    break;
	} else {
	    if (t - p) {
		char ssp[t - p];
		ssp[t - p] = 0;
		strncpy(ssp, p, t - p);
		ans = json_object_get(ans, ssp);
	    }
	}
	p = t + 1;
    }
    return ans;
}

int wd_reader(int argsc, char **args)
{
#define wrset(x) (opt|=(1<<x))
#define wriset(x) (opt&(1<<x))
    char *path = "/files/young/Cw/";
    char *stitle = NULL;

    if (argsc == 1) {
	wr_help();
	return 1;
    }

    while (1) {
	int c;
	c = getopt_long(argsc, args, "hp:ltT:", WR_Opts, NULL);
	if (-1 == c)
	    break;

	switch (c) {
	case 'h':
	    wr_help();
	    return 0;
	    break;
	case 'l':
	    wrset(1);
	    break;
	case 't':
	    wrset(2);
	    break;
	case 'p':
	    path = strdup(optarg);
	    break;
	case 'T':
	    stitle = strdup(optarg);
	    break;
	}
    }

    if (!path) {
	path = getenv("WIKIS_PATH");
    }
    if (!path) {
	fprintf(stderr, "Consider specifying the --path, option.\n");
	return 1;
    }
    json_t *j;
    struct stat st;

    if (stat(path, &st))
	return 2;

    if (!wrset(1) && !stitle) {
	fprintf(stderr,
		"Consider specifying the, --title, -T <arg> option.\n");
	return 2;
    }

    if (S_ISDIR(st.st_mode)) {
	DIR *d = opendir(path);
	struct dirent *ent;

	if (!d)
	    return 3;
	while (1) {
	    ent = readdir(d);
	    if (!ent)
		break;
	    if (0 == strncmp("..", ent->d_name, strlen(ent->d_name)))
		continue;
	    char *pa = NULL;
	    asprintf(&pa, "%s/%s", path, ent->d_name);
	    if (!stat(pa, &st) && !S_ISDIR(st.st_mode)
		&& !getCOMPRESSED_JSON(pa, &j)) {
		j = get_s_str(j, "query.pages");
		json_dumpf(j, stdout, 0);
		//puts(pa);
	    }
	}
	closedir(d);
    } else {
	if (!getCOMPRESSED_JSON(path, &j)) {
	    puts(path);
	}
    }
    return 0;
}

/* }}} */

// Main applets Declaration
/* {{{ */
struct Mopts MainMS[] = {
    { WK_DOWNLOAD, "download|get|dw|ge", wk_download,
     "The Wiki Downloader." },
    { WK_VIEW, "reader|viewer|opener|surfer", wd_reader,
     "The Wiki Reader." },
    {.str = NULL,.main = NULL }
};

static void MA_help(char *prog)
{
    struct Mopts *p = MainMS;
    fprintf(stderr, "%s [applet] [args]\n", prog);
    fprintf(stderr, "Where [applet] is either of:\n");
    while (p) {
	if (!p->str)
	    break;
	if (p->main) {
	    fprintf(stderr, "%d)\t(%s) for [%s]\n",
		    (p - MainMS) + 1, p->str, p->info);
	}
	p++;
    }
}

/* }}} */

// Main
int main(signed Argsc, char *(Args[]))
/* {{{ */
{
    char **ap = Args + 1;
    int ar = Argsc - 1;
    struct Mopts *p = MainMS;

    if (!ap || !*ap) {
	MA_help(*Args);
	return 1;
    }

    while (p) {
	if (p->str == NULL)
	    break;
	char *sp = p->str;
	while (sp) {
	    char *ex = NULL;
	    char *tmp = strchr(sp, '|');

	    if (!tmp) {
		ex = sp;
	    } else {
		asprintf(&ex, "%.*s", tmp - sp, sp);
	    }
	    if (0 == strncasecmp(ex, *ap, strlen(*ap))) {
		return p->main(ar, ap);
	    }
	    if (!tmp)
		break;
	    free(ex);
	    sp = tmp + 1;
	}
	p++;
    }
    return 0;
}

static void x_show_text(char *t)
/* {{{ */
{
    char *p = t;

    while (p && *p) {
	if (!isspace(*p))
	    break;
	p++;
    }

    while (p && *p) {
	if (*p > 0) {
	    putchar(*p);
	}
	p++;
    }
    putchar(10);
}

static void noku(char *ti, char *txt, char *tI)
{
    if (!tI || 0 == strncasecmp(tI, ti, strlen(tI))) {
	fprintf(stdout, "%s\n", ti);
	if (tI) {
	    char *p = txt;

	    while (p && *p) {
		char *tmp = strchr(p, '.');

		if (!tmp) {
		    x_show_text(p);
		    break;
		} else {
		    char *ddp = NULL;
		    asprintf(&ddp, "%.*s", tmp - p + 1, p);
		    x_show_text(ddp);
		    free(ddp);
		}

		p = tmp + 1;
	    }
	}
    }
}
