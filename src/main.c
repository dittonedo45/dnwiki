/*XXX This Document was modified on 1635407983 */
// Headers
/* {{{ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#include <jv.h>
#include <zlib.h>
#include <getopt.h>
#include <sys/stat.h>
/* }}} */
/*Misc*/
/* {{{ */

typedef
    struct {
    char *data;
    int size;

    CURL *hand;
    double lastruntime;

} Mem;

static void mem_init(Mem * pa)
{
    pa->data = malloc(1);
    pa->size = 0;
}

static void mem_free(Mem * pa)
{
    free(pa->data);
    pa->data = NULL;
    pa->size = -1;
}

static size_t myWRITER(data, a, b, stream)
void *data, *stream;
size_t a, b;
{
    size_t rs = a * b;
    Mem *mem = (Mem *) stream;
    mem->data = realloc(mem->data, mem->size + rs + 2);

    memcpy(&(mem->data[mem->size]), data, rs);

    mem->size += rs;

    mem->data[mem->size] = 0;
    return rs;
}

/* }}} */

/*Download*/
/* {{{ */
static int progress(p, dltotal, dlnow, ultotal, ulnow)
void *p;
double dltotal, dlnow, ultotal, ulnow;
{
    Mem *mm = (Mem *) p;

    CURL *cc = mm->hand;
    double cur = 0;

    curl_easy_getinfo(cc, CURLINFO_TOTAL_TIME, &cur);

    fprintf(stderr, "%lf %lf\n", dltotal, dlnow);

    return 0;
}

/* }}} */

/*Compressed File Dealing*/
int myCOMPRESSOR(void *data, size_t size)
/* {{{ */
{
    char *path_dowry = ".Cw/";
    mkdir(path_dowry, 0755);
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
    gzwrite(file, data, size);
    gzflush(file, Z_FINISH);
    gzclose(file);

    return 0;
}

static int myDD_AND_SAVER(char *url, CURLcode * rrs, jv * jdata)
{
    CURL *download;
    CURLcode res;
    Mem page;
    jv jj;

    mem_init(&page);

    res = curl_global_init(CURL_GLOBAL_ALL);

    if (res == 0) {
	download = curl_easy_init();
	if (download) {

	    page.hand = download;
	    page.lastruntime = 0.0f;

	    curl_easy_setopt(download, CURLOPT_URL, url);
	    curl_easy_setopt(download, CURLOPT_WRITEFUNCTION, myWRITER);
	    curl_easy_setopt(download, CURLOPT_USERAGENT,
			     "Ditto/Nedo's The Bibliophilist");

	    curl_easy_setopt(download, CURLOPT_WRITEDATA, &page);
	    res = curl_easy_perform(download);
	    curl_easy_cleanup(download);

	    if (!res) {
		jj = jv_parse(page.data);
		if (jv_is_valid(jj)) {
		    if (jdata)
			*jdata = jj;
		    if (myCOMPRESSOR(page.data, page.size)) {
			free(page.data);
			return 6;
		    }
		} else
		    return 4;
		//jv_free ( jj );
		free(page.data);
		return 0;
	    } else {
		if (rrs)
		    *rrs = res;
		return 3;
	    }
	} else
	    return 2;
    } else
	return 1;
}

#include <dirent.h>
int getCOMPRESSED_JSON(char *url, jv * data)
{
    gzFile file;

    if (!url)
	return 1;
    file = gzopen(url, "r");
    if (!file) {
	fprintf(stderr, "Failed To Open %s.\n", url);
	return 1;
    }

    char *buff = malloc(1);
    int size = 0;

    while (!gzeof(file)) {
	char buf[1054];
	gzgets(file, &*buf, 1054);
	int lsz = strlen(buf);
	{
	    buff = realloc(buff, (lsz + size + 1) * sizeof(char));
	    memcpy(&buff[size], buf, lsz);
	    size += lsz;
	    buff[size] = 0;
	}
    }

    if (data) {
	*data = jv_parse(buff);
    }
    free(buff);
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

#include <sox.h>

int wk_download(int argsc, char *(args[]))
{
    char **urls = NULL;
    char *lang = NULL;
    char *type = NULL;
    jv j;
    CURLcode r;

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
	    sox_append_comments(&urls, optarg);
	    break;
	case 'l':
	    lang = strdup(optarg);
	    break;
	case 't':
	    type = strdup(optarg);
	    break;
	}
    }
    char **p = urls;

    while (p) {
	if (!*p)
	    break;
	char *tp = *p;

	while (tp) {
	    char *exxt = NULL;
	    char *tmp = strchr(tp, ',');

	    if (!tmp) {
		exxt = tp;
	    } else {
		asprintf(&exxt, "%.*s", tmp - tp, tp);
	    }
	  tt:
	    {
		char *sp = strtourl(exxt);
		if (!myDD_AND_SAVER
		    (wikipath
		     (sp, type ? type : "pedia", lang ? lang : "en"), &r,
		     &j)) {
		    fprintf(stderr, "OK (%s)\n", exxt);
		} else {
		    free(sp);
		    goto tt;
		}
		free(sp);
	    }

	    if (!tmp)
		break;
	    free(exxt);
	    tp = tmp + 1;
	}
	p++;
    }
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
	fprintf(stderr, "\toption --%s, -%c %s\n", p->name, p->val,
		p->has_arg ? "<arg>" : "Flag");
	p = p + 1;
    }
}

static jv getObject(jv a, char *str)
{
    return jv_copy(jv_object_get(a, jv_string(str)));
}

static int get_title(jv data, char **title)
{
    jv p = jv_copy(data);
    if (!jv_is_valid(p))
	return 0;
    jv ti = getObject(p, "title");

    *title = strdup(jv_string_value(ti));
    jv_free(p);

    return !!*title;
}

static void deal_with(jv j, char *st);

int opt = 0;
int wd_reader(int argsc, char **args)
{
#define wrset(x) (opt|=(1<<x))
#define wriset(x) (opt&(1<<x))
    char *path = ".Cw/";
    mkdir(path, 0755);
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
    jv j;
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
#define _(x) jv_copy(x)
#define t(x) jv_kind_name(jv_get_kind (x))
#define p(x) jv_string(x)
		deal_with(_(j), stitle);

		//puts ( pa );
	    }
	}
	closedir(d);
    } else {
	if (!getCOMPRESSED_JSON(path, &j)) {
	    puts(path);
	}
    }
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
	    fprintf(stderr, "%d)\t(%s) for [%s]\n", (p - MainMS) + 1,
		    p->str, p->info);
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

/* }}} */

int read_pages(jv a, int *comment, jv * pages)
/* {{{ */
{
    int i = 0;
    int j = 0;
    jv p = jv_copy(a);
    jv el;
#if 0

    if (!jv_get_kind(p) == JV_KIND_ARRAY) {
	*comment = -3;
	return 0;
    }

    jv_array_foreach(p, j, el) {
	if (j > 100) {
	    *comment = -1;
	    break;
	}
#endif
	jv tmp = getObject(el, "query");
	if (!jv_is_valid(tmp)) {
	    *comment = -4;
	    //break;
	    return 0;
	}
	jv key, val;

	tmp = getObject(tmp, "pages");

	jv_object_foreach(tmp, key, val) {
	    (pages)[j++] = jv_copy(val);
	    i++;
	}
#if 0
    }
#endif
    *comment = 0;
    return i;
}

/* }}} */
/*Print Untils*/
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

/* }}} */

static void deal_with(jv j, char *tiI)
/* {{{ */
{
#if 1
    //jv_dumpf ( j, stdout, 0 );
    jv query = _(jv_object_get(j, p("query")));
    jv pages = _(jv_object_get(query, p("pages")));

    jv_object_foreach(pages, id, dd) {
	jv cirrusdoc = _(jv_object_get(dd, p("cirrusdoc")));

	if (jv_is_valid(cirrusdoc)) {

	    jv title = _(jv_object_get(dd, p("title")));
	    if (jv_is_valid(title)) {
		char *s = strdup(jv_string_value(title));
		jv_array_foreach(cirrusdoc, i, tmp) {
		    jv src = _(jv_object_get(tmp, p("source")));
		    jv txt = _(jv_object_get(src, p("text")));

		    char *text = strdup(jv_string_value(txt));
		    noku(s, text, tiI);
		    free(text);
		}
		free(s);
		//jv_dumpf ( cirrusdoc, stdout, 0 );
	    }
//   jv_free ( title );
	}
//  jv_free ( cirrusdoc );
    }
// jv_free ( query );
// jv_free ( pages );
// jv_free ( j );
    //exit ( 1 );
#else
    jv pp[9000];
    int r = 0;
    read_pages(j, &r, pp);
#endif
}

/* }}} */
