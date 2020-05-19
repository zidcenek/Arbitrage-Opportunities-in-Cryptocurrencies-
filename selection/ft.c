#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <err.h>

#define MAX(_a, _b)	(((_a) > (_b)) ? (_a) : (_b))
#define ABS(_f)		(((_f) > 0 ) ? (_f) : (-(_f)))

struct coin {
	const char *name;
	struct cpair {
		struct pair  *pair;
		struct cpair *next; /* in the coin's list */
	} *pairs;
} coins[] = {
	{ "BCH",  NULL },
	{ "BNB",  NULL },
	{ "BTC",  NULL },
	{ "EOS",  NULL },
	{ "ETH",  NULL },
	{ "LTC",  NULL },
	{ "TRX",  NULL },
	{ "XMR",  NULL },
	{ "XRP",  NULL },
	{ "USDT", NULL },
	{  NULL,  NULL },
};

struct deal {
	double price;
	double amount;
};

struct book {
	double ts;
	struct deal bid[5];
	struct deal ask[5];
};

struct blog {
	double ts;
	char* path;
	FILE* file;
	struct blog *next;
};

struct pair {
	int used;
	struct coin	*a;
	struct coin	*b;
	struct book	*book;	/* latest */
	struct blog	*blog;	/* reading now */
	struct blog	*blogs; /* list of all */
	struct ptgle {
		struct triangle	*tgle;
		struct pair	*pair; /* this pair of the triangle */
		struct ptgle	*next; /* in the pair's list */
	} *tgles;
	struct pair	*next;
} *pairs = NULL;

struct triangle {
	struct coin *X;
	struct coin *Y;
	struct coin *Z;
	struct pair* XY;
	struct pair* YZ;
	struct pair* ZX;
	struct triangle *next;
} *triangles = NULL;

struct coin*
getcoin(const char* name)
{
	struct coin* c = coins;
	while (c && c->name) {
		if (0 == strncmp(c->name, name, 3))
			return c;
		c++;
	}
	return NULL;
}

struct pair*
getpair(struct coin *a, struct coin *b)
{
	struct pair *p;
	for (p = pairs; p; p = p->next) {
		if (0 == strcmp(a->name, p->a->name)
		&&  0 == strcmp(b->name, p->b->name)) {
			return p;
		}
	}
	return NULL;
}

void
tellpair(struct pair *p)
{
	struct blog *b;
	if (NULL == p)
		return;
	printf("%s:%s %f\n", p->a->name, p->b->name, p->book->ts);
	for (b = p->blogs; b; b = b->next) {
		printf("\t%f in %s\n", b->ts, b->path);
	}
}

void
tellpairs()
{
	struct pair *p;
	for (p = pairs; p ; p = p->next)
		tellpair(p);
}

struct book*
readbook(FILE* file)
{
	ssize_t len;
	size_t size = 0;
	char *p, *line = NULL;
	struct book* book;
	if (-1 == (len = getline(&line, &size, file))) {
		warn("getstamp");
		return NULL;
	}
	p = line;
	if (NULL == (book = calloc(1, sizeof(struct book))))
		err(1, NULL);
	/* FIXME parse the book */
	strsep(&line, ";");
	strsep(&line, ";");
	strsep(&line, ";");
	book->ts = strtod(line, NULL);
	free(p);
	return book;
}

void
addblog(struct pair *pair, const char *dir, const char *file)
{
	char name[1024];
	struct blog *blog;
	struct blog *prev;
	struct blog *this;
	struct book *book;
	if (NULL == pair)
		return;
	if (NULL == dir || NULL == file) {
		warnx("blog file not specified\n");
		return;
	}
	if (NULL == (blog = calloc(1, sizeof(struct blog))))
		err(1, NULL);
	snprintf(name, 1024, "%s/%s", dir, file);
	blog->path = realpath(name, NULL);
	if (NULL == (blog->file = fopen(name, "r"))) {
		warn("cannot open %s\n", blog->path);
		return;
	}
	if (NULL == (book = readbook(blog->file))) {
		warnx("cannot read book from of %s\n", blog->path);
		return;
	}
	fclose(blog->file);
	blog->file = NULL;
	blog->ts = book->ts;
	if (NULL == pair->blogs) {
		pair->blogs = blog;
		return;
	}
	prev = NULL;
	this = pair->blogs;
	while (this && this->ts < blog->ts) {
		prev = this;
		this = this->next;
	}
	if (prev) {
		prev->next = blog;
		blog->next = this;
	} else {
		pair->blogs = blog;
		blog->next = this;
	}
}

struct pair*
mkpair(struct coin *a, struct coin *b)
{
	struct pair *p;
	if (NULL == (p = calloc(1, sizeof(struct pair))))
		err(1, NULL);
	p->a = a;
	p->b = b;
	return p;
}

void
newblog(struct coin *a, struct coin *b, const char *dir, const char *file)
{
	struct pair *p;
	if ((p = getpair(a, b))) {
		addblog(p, dir, file);
		return;
	}
	if (NULL == (p = mkpair(a, b))) {
		warnx("cannot create %s %s", a->name, b->name);
		return;
	}
	addblog(p, dir, file);
	p->next = pairs;
	pairs = p;
}

/* Add a pair to a coin's list of pairs */
void
addcpair(struct pair *p, struct coin *c)
{
	struct cpair *cp;
	if (NULL == p || NULL == c)
		return;
	if (NULL == (cp = calloc(1, sizeof(struct cpair))))
		err(1, NULL);
	cp->pair = p;
	cp->next = c->pairs;
	c->pairs = cp;
}

/* Go through the global list of pairs
 * and list the relevant pairs in each coin */
void
mkcpairs()
{
	struct pair *p;
	for (p = pairs; p; p = p->next) {
		addcpair(p, p->a);
		addcpair(p, p->b);
	}
}

/* Add triangle to a pairs's list of triangles */
void
addtgle(struct triangle *t, struct pair *p)
{
	struct ptgle *pt;
	if (NULL == t || NULL == p)
		return;
	if (NULL == (pt = calloc(1, sizeof(struct ptgle))))
		err(1, NULL);
	pt->tgle = t;
	pt->pair = p;
	pt->next = p->tgles;
	p->tgles = pt;
}

void
newtriangle(
	struct coin *X, struct coin *Y, struct coin *Z,
	struct cpair *xp, struct cpair *yp, struct cpair *zp)
{
	struct triangle *t;
	if (NULL == (t = calloc(1, sizeof(struct triangle))))
		err(1, NULL);
	t->X = X;
	t->Y = Y;
	t->Z = Z;
	t->XY = xp->pair;
	t->YZ = yp->pair;
	t->ZX = zp->pair;
	/* Add to the global list of triangles. */
	t->next = triangles;
	triangles = t;
	/* Add to the relevant pairs' lists. */
	addtgle(t, xp->pair);
	addtgle(t, yp->pair);
	addtgle(t, zp->pair);
}

/* Go through the coins' pairs and build all triangles */
void
tribuild()
{
	struct coin *X;
	struct coin *Y;
	struct coin *Z;
	struct cpair *xp;
	struct cpair *yp;
	struct cpair *zp;
	for (X = coins; X && X->name; X++) {
		/*printf("%s triangles\n", X->name);*/
		for (xp = X->pairs; xp; xp = xp->next) {
			if (xp->pair->used++)
				continue;
			Y = X == xp->pair->a ? xp->pair->b : xp->pair->a;
			/*printf("\tvia %s\n", Y->name);*/
			for (yp = Y->pairs; yp; yp = yp->next) {
				if (yp->pair->used)
					continue;
				Z = Y==yp->pair->a ? yp->pair->b : yp->pair->a;
				/*printf("\t\tvia %s\n", Z->name);*/
				for (zp = Z->pairs; zp; zp = zp->next) {
					if (zp->pair->used)
						continue;
					if (X == zp->pair->a
					||  X == zp->pair->b) {
						newtriangle(X,Y,Z,xp,yp,zp);
						newtriangle(X,Z,Y,zp,yp,xp);
						break;
					}

				}
			}
		}
	}
}

/* A triangle is synced if the timestamps of all its pairs' books
 * are within half a second of each other, i.e., no two are more
 * than half a second apart. Such a triangle might be traded. */
int
synced(struct triangle *t)
{
	return
		ABS(t->XY->book->ts - t->YZ->book->ts) < 0.5 &&
		ABS(t->YZ->book->ts - t->ZX->book->ts) < 0.5 &&
		ABS(t->ZX->book->ts - t->XY->book->ts) < 0.5
	;
}

void
telltriangle(struct triangle *t)
{
	if (NULL == t)
		return;
	printf("%s:%s:%s", t->X->name, t->Y->name, t->Z->name);
	if (synced(t)) {
		printf(" (synced @ %f)",
			MAX(t->XY->book->ts,
			MAX(t->YZ->book->ts, t->ZX->book->ts)));
	}
	putchar('\n');
}

void
telltrgls()
{
	struct triangle *t;
	for (t = triangles; t ; t = t->next)
		telltriangle(t);
}

void
tellcoin(struct coin *c)
{
	struct cpair *cp;
	printf("%s is in", c->name);
	for (cp = c->pairs; cp; cp = cp->next)
		printf(" %s:%s", cp->pair->a->name, cp->pair->b->name);
	putchar('\n');
}

void
tellcoins()
{
	struct coin *c;
	for (c = coins; c && c->name; c++)
		tellcoin(c);
}

/* Go through the global list of pairs,
 * open the first book log and read in the first order book. */
void
initbooks()
{
	struct pair *p;
	for (p = pairs; p; p = p->next) {
		p->blog = p->blogs;
		if (NULL == (p->blog->file = fopen(p->blog->path, "r"))) {
			warn("cannot open %s", p->blog->path);
			p->blog = NULL;
			continue;
		}
		if (NULL == (p->book = readbook(p->blog->file))) {
			warn("cannot read book from %s", p->blog->path);
			continue;
		}
		/* FIXME If this was the last book in the log,
		 * fclose(path) and fopen() the new one, so that
		 * the last read happens transparently. */
	}
}

void
usage()
{
	fprintf(stderr, "usage: %s bookdir\n", "ft");
}

int
main(int argc, char** argv)
{
	DIR *d;
	struct dirent *bf;
	struct coin *a, *b;

	if (argc < 2) {
		usage();
		return 1;
	}

	if (NULL == (d = opendir(argv[1]))) {
		err(1, "Cannot open dir %s", argv[1]);
	}

	while ((bf = readdir(d))) {
		if (bf->d_namlen < 7)
			continue;
		if (NULL == (a = getcoin(bf->d_name)))
			continue;
		if (NULL == (b = getcoin(bf->d_name + strlen(a->name))))
			continue;
		newblog(a, b, argv[1], bf->d_name);
	}
	closedir(d);

	mkcpairs();
	tribuild();
	initbooks();

	tellpairs();
	tellcoins();
	telltrgls();

	return 0;
}
