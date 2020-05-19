#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <err.h>

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

struct book {
	char* path;
	FILE* file;
	double start;
	struct book *next;
};

struct pair {
	int used;
	struct coin *a;
	struct coin *b;
	struct book *books;
	struct pair *next;
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
	struct book *b;
	if (NULL == p)
		return;
	printf("%s:%s\n", p->a->name, p->b->name);
	for (b = p->books; b; b = b->next) {
		printf("\t%f in %s\n", b->start, b->path);
	}
}

void
tellpairs()
{
	struct pair *p;
	for (p = pairs; p ; p = p->next)
		tellpair(p);
}

int
getstamp(struct book *book)
{
	ssize_t len;
	size_t size = 0;
	char *p, *line = NULL;
	if (-1 == (len = getline(&line, &size, book->file))) {
		warn("getstamp");
		return -1;
	}
	p = line;
	strsep(&line, ";");
	strsep(&line, ";");
	strsep(&line, ";");
	book->start = strtod(line, NULL);
	free(p);
	return 0;
}

void
addbook(struct pair *pair, const char *dir, const char *file)
{
	char name[1024];
	struct book *book;
	struct book *prev;
	struct book *this;
	if (NULL == pair)
		return;
	if (NULL == dir || NULL == file) {
		warnx("book file not specified\n");
		return;
	}
	if (NULL == (book = calloc(1, sizeof(struct book))))
		err(1, NULL);
	snprintf(name, 1024, "%s/%s", dir, file);
	book->path = realpath(name, NULL);
	if (NULL == (book->file = fopen(name, "r"))) {
		warn("cannot open %s\n", book->path);
		return;
	}
	if (getstamp(book) == -1) {
		warnx("cannot get timestamp of %s\n", book->path);
		return;
	}
	fclose(book->file);
	if (NULL == pair->books) {
		pair->books = book;
		return;
	}
	prev = NULL;
	this = pair->books;
	while (this && this->start < book->start) {
		prev = this;
		this = this->next;
	}
	if (prev) {
		prev->next = book;
		book->next = this;
	} else {
		pair->books = book;
		book->next = this;
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
newbook(struct coin *a, struct coin *b, const char *dir, const char *file)
{
	struct pair *p;
	if ((p = getpair(a, b))) {
		addbook(p, dir, file);
		return;
	}
	if (NULL == (p = mkpair(a, b))) {
		warnx("cannot create %s %s", a->name, b->name);
		return;
	}
	addbook(p, dir, file);
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
	t->next = triangles;
	triangles = t;
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

void
telltriangle(struct triangle *t)
{
	if (NULL == t)
		return;
	printf("%s/%s/%s via %s:%s, %s:%s, %s:%s\n",
		t->X->name, t->Y->name, t->Z->name,
		t->XY->a->name, t->XY->b->name,
		t->YZ->a->name, t->YZ->b->name,
		t->ZX->a->name, t->ZX->b->name);
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
		newbook(a, b, argv[1], bf->d_name);
	}
	closedir(d);

	mkcpairs();
	tribuild();

	tellpairs();
	tellcoins();
	telltrgls();

	return 0;
}
