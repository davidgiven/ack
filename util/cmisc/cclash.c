/*
	cclash: find clashing names within C programs

	Flags:
	-c		: produce oldname=newname line for each clashing id
			  (useful input for cid)
	-l<num>		: check identifiers of <num> or more characters
			  (default <num> = 8)
	-m		: output a #define for each clashing id

	Author: Erik Baalbergen
	Date: Nov 8, 1985

	Revised: Dec 10, 1985
		C keywords are not listed
	Revised: Aug 27, 1986
		Skip C numeric constants
	Revised: Wed Jul 23 13:27:16 MDT 1986
		by Ceriel Jacobs,
		replaced "stoi" by "atoi"
	Revised: Wed Oct  1 14:23:35 MDT 1986
		by Ceriel Jacobs,
		to produce lists in the order in which they were read.
*/

#include <stdio.h>

#define DEF_LENGTH	8

struct idf {
	struct idf *id_next;
	char *id_name;
	struct idf *id_same;
	char *id_key;
};

#define ACT_LISTONLY	0
#define ACT_MAPFILE	1
#define ACT_CID		2

int maxlen = DEF_LENGTH;
int action = ACT_LISTONLY;

char *ProgName;

char * keywords[] = {
	"asm",
	"auto",
	"break",
	"case",
	"char",
	"continue",
	"default",
	"do",
	"double",
	"else",
	"entry",
	"extern",
	"float",
	"for",
	"fortran",
	"goto",
	"if",
	"int",
	"long",
	"register",
	"return",
	"short",
	"sizeof",
	"static",
	"struct",
	"switch",
	"typedef",
	"union",
	"unsigned",
	"while",
	0
};

DefineKeys()
{
	register char **pkey = &keywords[0];
	register char *id;

	while (id = *pkey++)
		if (strlen(id) >= maxlen)
			InsertId(id, 1);
}

DoOption(str)
	char *str;
{
	switch (str[1]) {

	case 'c':
		action = ACT_CID;
		break;

	case 'l':
		if ((maxlen = atoi(&str[2])) <= 0) {
			fprintf(stderr, "%s: option \"-l%s\" ignored\n",
				ProgName, &str[2]);
			maxlen = DEF_LENGTH;
		}
		break;
	
	case 'm':
		action = ACT_MAPFILE;
		break;

	default:
		fprintf(stderr, "%s: bad option \"%s\"\n", ProgName, str);
		break;
	}
}

#define HASHSIZE 257

struct idf *hash_tab[HASHSIZE];

char *Malloc(), *Salloc();

InsertId(id, key)
	char *id;
{
	int hash_val = EnHash(id);
	register struct idf *idp = hash_tab[hash_val];
	register struct idf *p = 0;

	while (idp && strncmp(idp->id_name, id, maxlen)) {
		p = idp;
		idp = idp->id_next;
	}

	if (idp == 0) {
		idp = (struct idf *) Malloc(sizeof(struct idf));
		idp->id_next = 0;
		if (!p) hash_tab[hash_val] = idp;
		else p->id_next = idp;
		idp->id_name = Salloc(id);
		idp->id_same = 0;
	}

	p = idp;

	while (p && strcmp(p->id_name, id)) {
		idp = p;
		p = p->id_same;
	}

	if (p == 0) {
		p = (struct idf *) Malloc(sizeof(struct idf));
		p->id_next = 0;
		p->id_same = 0;
		p->id_name = Salloc(id);
		idp->id_same = p;
	}

	if (key)
		p->id_key = id;
}

char *
Malloc(n)
	unsigned n;
{
	char *mem, *malloc();

	if ((mem = malloc(n)) == 0) {
		fprintf(stderr, "%s: out of memory\n", ProgName);
		exit(1);
	}
	return mem;
}

char *
Salloc(str)
	char *str;
{
	char *strcpy();

	if (str == 0)
		str = "";

	return strcpy(Malloc(strlen(str) + 1), str);
}

EnHash(id)
	char *id;
{
	register unsigned hash_val = 0;
	register n = maxlen;

	while (n-- && *id)
		hash_val = 31 * hash_val + *id++;

	return hash_val % (unsigned) HASHSIZE;
}

Results()
{
	register int i;
	register struct idf *idp, *p;

	for (i = 0; i < HASHSIZE; i++) {
		for (idp = hash_tab[i]; idp; idp = idp->id_next) {
			if (idp->id_same == 0)
				continue;

			switch (action) {
				register n;

			case ACT_LISTONLY:
				n = 0;
				if (idp->id_key == 0) {
					printf(idp->id_name);
					n++;
				}
				for (p = idp->id_same; p; p = p->id_same)
					if (p->id_key == 0) {
						printf("%s%s",
							n ? " " : "",
							p->id_name
						);
						n++;
					}
				if (n)
					printf("\n");
				break;
			
			case ACT_CID:
			case ACT_MAPFILE:
				if (idp->id_key == 0)
					mapline(idp->id_name);
				for (p = idp->id_same; p; p = p->id_same)
					if (p->id_key == 0)
						mapline(p->id_name);
				break;
			}
		}
	}
}

mapline(nm)
	char *nm;
{
	static map_count = 0;

	switch (action) {

	case ACT_MAPFILE:
		printf("#define %s _%d_%s\n", nm, ++map_count, nm);
		break;
	
	case ACT_CID:
		printf("%s=_%d_%s\n", nm, ++map_count, nm);
		break;
	}
}

#define MAX_ID_LEN	256

main(argc, argv)
	char *argv[];
{
	char **nargv;
	int nargc = 0;
	FILE *fp;

	ProgName = *argv++;
	nargv = argv;

	while (--argc > 0)
		if ((*argv)[0] == '-')
			DoOption(*argv++);
		else
			nargv[nargc++] = *argv++;

	DefineKeys();
	if (nargc > 0)
		while (nargc-- > 0) {
			if ((fp = fopen(*nargv, "r")) == NULL)
				fprintf(stderr, "%s: cannot read file \"%s\"\n",
					ProgName, *nargv);
			else
				DoFile(fp);
			nargv++;
		}
	else
		DoFile(stdin);
	Results();
	return 0;
}

DoFile(fp)
	FILE *fp;
{
	register c;

	while ((c = getc(fp)) != EOF)
		switch (c) {

		case '"':
		case '\'':
			SkipString(fp, c);
			break;
		
		case '/':
			if ((c = getc(fp)) == '*')
				SkipComment(fp);
			else
				ungetc(c, fp);
			break;

		default:
			if (StartId(c))
				DoIdent(fp, c);
			else
			if (StartNum(c))
				DoNum(fp, c);
			break;
		}

	fclose(fp);
}

SkipString(fp, stopc)
	FILE *fp;
{
	register c;

	while ((c = getc(fp)) != EOF) {
		if (c == stopc)
			return;
		if (c == '\\')
			c = getc(fp);
	}
}

SkipComment(fp)
	FILE *fp;
{
	register c;

	while ((c = getc(fp)) != EOF)
		if (c == '*') {
			if ((c = getc(fp)) == '/')
				return;
			ungetc(c, fp);
		}
}

DoIdent(fp, s)
	FILE *fp;
{
	char id_buf[MAX_ID_LEN];
	register cnt = 1;
	register c;

	id_buf[0] = s;

	while ((c = getc(fp)) != EOF)
		if (InId(c))
			id_buf[cnt++] = c;
		else {
			ungetc(c, fp);
			id_buf[cnt] = '\0';
			if (cnt >= maxlen)
				InsertId(&id_buf[0], 0);
			return;
		}
}

#define inrange(c, l, u)	((unsigned)((c) - (l)) <= ((u) - (l)))
#define isdec(c) inrange(c, '0', '9')
#define isoct(c) inrange(c, '0', '7')
#define ishex(c) (isdec(c) || inrange(c, 'a', 'f') || inrange(c, 'A', 'F'))
#define getdec(c, fp)	do c = getc((fp)); while (isdec(c))
#define getoct(c, fp)	do c = getc((fp)); while (isoct(c))
#define gethex(c, fp)	do c = getc((fp)); while (ishex(c))

DoNum(fp, c)
	FILE *fp;
{
	if (c != '0') {
		getdec(c, fp);
		if (c == '.')
			getdec(c, fp);
		if (c == 'e') {
			c = getc(fp);
			if (c == '+' || c == '-')
				c = getc(fp);
			if (isdec(c))
				getdec(c, fp);
		}
	}
	else {
		c = getc(fp);
		if (c == 'x' || c == 'X')
			gethex(c, fp);
		else
		if (isoct(c))
			getoct(c, fp);
	}
}

StartId(c)
{
	switch (c) {

	case 'a': case 'b': case 'c': case 'd': case 'e':
	case 'f': case 'g': case 'h': case 'i': case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'o':
	case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y':
	case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E':
	case 'F': case 'G': case 'H': case 'I': case 'J':
	case 'K': case 'L': case 'M': case 'N': case 'O':
	case 'P': case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X': case 'Y':
	case 'Z':
	case '_':
		return 1;
	
	default:
		return 0;
	}
}

StartNum(c)
{
	switch(c) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return 1;
	}
	return 0;
}

InId(c)
{
	switch (c) {

	case 'a': case 'b': case 'c': case 'd': case 'e':
	case 'f': case 'g': case 'h': case 'i': case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'o':
	case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y':
	case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E':
	case 'F': case 'G': case 'H': case 'I': case 'J':
	case 'K': case 'L': case 'M': case 'N': case 'O':
	case 'P': case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X': case 'Y':
	case 'Z':
	case '_':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return 1;
	
	default:
		return 0;
	}
}
