/*	@cc % $LIBDIR/stoi.o -o cid@	*/

/*	Change IDentifiers occurring in C programs outside comment, strings
	and character constants.
	-Dname=text	: replace all occerences of name by text
	-Dname		: same as -Dname=
	-Ffile		: read sentences of the from name=text from file

	Author: Erik Baalbergen
	Date: Oct 23, 1985
*/


/*** Generic C Identifier Processing Module	***/
/* IMPORT CheckId(char *) and DoOption(char *, int)
*/

#include <stdio.h>

extern CheckId();
extern DoOption();

#define MAX_ID_LEN	256

char *ProgName;

main(argc, argv)
	char *argv[];
{
	char **nargv;
	int nargc = 0;
	FILE *fp;

	ProgName = *argv++;
	nargv = argv;

	while (--argc > 0) {
		if ((*argv)[0] == '-') {
			DoOption(*argv++);
		}
		else {
			nargv[nargc++] = *argv++;
		}
	}

	if (nargc > 0) {
		while (nargc-- > 0) {
			if ((fp = fopen(*nargv, "r")) == NULL) {
				fprintf(stderr, "%s: cannot read file \"%s\"\n",
					ProgName, *nargv);
			}
			else {
				DoFile(fp);
			}
			nargv++;
		}
	}
	else {
		DoFile(stdin);
	}
	return 0;
}

DoFile(fp)
	FILE *fp;
{
	register c;

	while ((c = getc(fp)) != EOF) {
		switch (c) {

		case '"':
		case '\'':
			SkipString(fp, c);
			break;
		
		case '/':
			if ((c = getc(fp)) == '*') {
				SkipComment(fp);
			}
			else {
				putchar('/');
				ungetc(c, fp);
			}
			break;

		default:
			if (StartId(c)) {
				DoIdent(fp, c);
			}
			else
				putchar(c);
			break;
		}
	}
	fclose(fp);
}

SkipString(fp, stopc)
	FILE *fp;
{
	register c;

	putchar(stopc);
	while ((c = getc(fp)) != EOF) {
		if (c == stopc) {
			putchar(stopc);
			return;
		}

		if (c == '\\') {
			putchar(c);
			c = getc(fp);
		}
		putchar(c);
	}
}

SkipComment(fp)
	FILE *fp;
{
	register c;

	putchar('/');
	putchar('*');
	while ((c = getc(fp)) != EOF) {
		if (c == '*') {
			putchar('*');
			if ((c = getc(fp)) == '/') {
				putchar('/');
				return;
			}
			ungetc(c, fp);
		}
		else
			putchar(c);
	}
}

DoIdent(fp, s)
	FILE *fp;
{
	char id_buf[MAX_ID_LEN];
	register cnt = 1;
	register c;

	id_buf[0] = s;

	while ((c = getc(fp)) != EOF) {
		if (InId(c)) {
			id_buf[cnt++] = c;
		}
		else {
			ungetc(c, fp);
			id_buf[cnt] = '\0';
			CheckId(id_buf, cnt);
			return;
		}
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
/*** end of GCIPM	***/

#ifndef DEF_LENGTH
#define DEF_LENGTH	8
#endif

#define LINE_LEN	1024

#define HASHSIZE 257

struct idf {
	struct idf *id_next;
	char *id_name;
	char *id_text;
};

struct idf *hash_tab[HASHSIZE];

char *Malloc(), *Salloc();
struct idf *FindId();

DoOption(str)
	char *str;
{
	switch (str[1]) {

	case 'D':
		DoMacro(&str[2]);
		break;
	
	case 'F':
		GetMacros(&str[2]);
		break;

	default:
		fprintf(stderr, "%s: bad option \"%s\"\n", ProgName, str);
		break;
	}
}

CheckId(id, len)
	char *id;
{
	struct idf *idp = FindId(id);

	if (idp) {
		printf("%s", idp->id_text);
	}
	else {
		printf("%s", id);
	}
}

DoMacro(str)
	char *str;
{
	char *id, *text;

	id = str++;

	while (*str != '\0' && *str != '=') {
		str++;
	}

	if (*str == '=') {
		*str++ = '\0';
		text = str;
	}
	else {
		text = "";
	}

	InsertMacro(id, text);
}

GetMacros(fn)
	char *fn;
{
	FILE *fp;
	register c;
	char buf[LINE_LEN];
	char *bufp = &buf[0];

	if ((fp = fopen(fn, "r")) == NULL) {
		fprintf(stderr, "%s: cannot read file \"%s\"\n", ProgName, fn);
		return 0;
	}

	while ((c = getc(fp)) != EOF) {
		if (c == '\n' && bufp != &buf[0]) {
			*bufp = '\0';
			DoMacro(&buf[0]);
			bufp = &buf[0];
		}
		else {
			*bufp++ = c;
		}
	}
	fclose(fp);
}

InsertMacro(id, text)
	char *id, *text;
{
	int hash_val = EnHash(id);
	struct idf *idp = hash_tab[hash_val];

	while (idp) {
		if (strcmp(idp->id_name, id) == 0) {
			fprintf(stderr, "%s: (warning) redefinition of %s\n",
				ProgName, id);
			break;
		}
		idp = idp->id_next;
	}

	if (idp == 0) {
		idp = (struct idf *) Malloc(sizeof(struct idf));
	}

	idp->id_next = hash_tab[hash_val];
	idp->id_name = Salloc(id);
	idp->id_text = Salloc(text);
	hash_tab[hash_val] = idp;
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

	if (str == 0) {
		str = "";
	}
	return strcpy(Malloc(strlen(str) + 1), str);
}

struct idf *
FindId(id)
	char *id;
{
	register hash_val = EnHash(id);
	register struct idf *idp = hash_tab[hash_val];

	while (idp) {
		if (strcmp(idp->id_name, id) == 0) {
			return idp;
		}
		idp = idp->id_next;
	}
	return 0;
}

EnHash(id)
	char *id;
{
	register unsigned hash_val = 0;

	while (*id) {
		hash_val = 31 * hash_val + *id++;
	}

	return hash_val % (unsigned) HASHSIZE;
}
