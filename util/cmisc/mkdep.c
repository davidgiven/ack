/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* make dependencies; Date: jan 07, 1986; Author: Erik Baalbergen */

#include <stdio.h>

#define BSIZ 1024
char *progname;

struct namelist {
	struct namelist *next;
	char *name;
};

struct namelist *freelist;
struct namelist *new_namelist();
struct namelist *nl = 0;

char *Malloc(u)
	unsigned u;
{
	char *sp, *malloc();

	if ((sp = malloc(u)) == 0) {
		fprintf(stderr, "%s: out of space\n");
		exit(1);
	}
	return sp;
}

struct namelist *
new_namelist()
{
	register struct namelist *nlp = freelist;

	if (nlp) {
		freelist = nlp->next;
		return nlp;
	}

	return (struct namelist *) Malloc(sizeof(struct namelist));
}

free_namelist(nlp)
	struct namelist *nlp;
{
	if (nlp) {
		free_namelist(nlp->next);
		nlp->next = freelist;
		freelist = nlp;
	}
}

add_name(nm)
	char *nm;
{
	struct namelist *nlp = nl;
	struct namelist *lnlp = 0;
	struct namelist *nnlp;
	char *strcpy();

	while (nlp) {
		register i = strcmp(nm, nlp->name);
		if (i < 0)
			break;
		if (i == 0)	/* already present */
			return;
		lnlp = nlp;
		nlp = nlp->next;
	}

	(nnlp = new_namelist())->name = strcpy(Malloc(strlen(nm) + 1), nm);

	if (lnlp) {
		nnlp->next = lnlp->next;
		lnlp->next = nnlp;
	}
	else {
		nnlp->next = nl;
		nl = nnlp;
	}
}

print_namelist(nm, nlp)
	char *nm;
	struct namelist *nlp;
{
	if (nlp) {
			while (nlp) {
				printf("%s: %s\n", nm, nlp->name);
				nlp = nlp->next;
			}
	}
}

main(argc, argv)
	char *argv[];
{
	int err = 0;

	progname = *argv++;
	while (--argc > 0) {
		free_namelist(nl);
		nl = 0;
		if (dofile(*argv) == 0)
			++err;
		print_namelist(*argv++, nl);
	}
	exit(err ? 1 : 0);
}

int
contains_slash(s)
	register char *s;
{
	while (*s) {
		if (*s++ == '/') return 1;
	}
	return 0;
}

dofile(fn)
	char *fn;
{
	char *fgets(), buf[BSIZ];
	FILE *fp;
	char *nm, *include_line();

	if ((fp = fopen(fn, "r")) == 0) {
		fprintf(stderr, "%s: cannot read %s\n", progname, fn);
		return 0;
	}

	if (contains_slash(fn)) {
		fprintf(stderr, "%s: (warning) %s not in current directory; not checked\n", progname, fn);
		fclose(fp);
		return 1;
	}

	while (fgets(buf, BSIZ, fp) != NULL)
		if (nm = include_line(buf)) {
			add_name(nm);
			dofile(nm);
		}

	fclose(fp);
	return 1;
}

char *
include_line(s)
	char *s;
{
	while ((*s == '\t') || (*s == ' '))
		s++;
	
	if (*s++ == '#') {
		while ((*s == '\t') || (*s == ' '))
			s++;
		if (
			(*s++ == 'i') &&
			(*s++ == 'n') &&
			(*s++ == 'c') &&
			(*s++ == 'l') &&
			(*s++ == 'u') &&
			(*s++ == 'd') &&
			(*s++ == 'e')
		) {
			while ((*s == '\t') || (*s == ' '))
				s++;
			if (*s++ == '"') {
				register char *nm = s;
				while (*s != 0 && *s != '"')
					s++;
				*s = '\0';
				return nm;
			}
		}
	}
	return (char *) 0;
}
