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

int lflag = 0;	/* produce vertical list of included files */

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
		if (lflag) {
			while (nlp) {
				printf("%s\n", nlp->name);
				nlp = nlp->next;
			}
		}
		else {
			printf("%s:", nm);
			while (nlp) {
				printf(" %s", nlp->name);
				nlp = nlp->next;
			}
			printf("\n");
		}
	}
}

main(argc, argv)
	char *argv[];
{
	int err = 0;

	progname = *argv++;
	if (**argv == '-') {
		register char *s = *argv++;

		argc--;
		while (*++s) {
			switch (*s) {
			case 'l':
				lflag = 1;
				break;
			default:
				fprintf(stderr, "use: %s [-l] file ...\n",
					progname);
				return 0;
			}
		}
	}
	while (--argc > 0) {
		free_namelist(nl);
		nl = 0;
		if (dofile(*argv) == 0)
			++err;
		print_namelist(*argv++, nl);
	}
	return err ? 1 : 0;
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
