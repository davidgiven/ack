/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	Change IDentifiers occurring in C programs outside comment, strings
	and character constants.
	-Dname=text	: replace all occerences of name by text
	-Dname		: same as -Dname=
	-Ffile		: read sentences of the from name=text from file

	Author: Erik Baalbergen
	Date: Oct 23, 1985
*/

#include <stdio.h>

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

extern char *ProgName;

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

/*ARGSUSED*/
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
		return;
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
	return strcpy(Malloc((unsigned)strlen(str) + 1), str);
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

extern int GCcopy;

BeginOfProgram()
{
	GCcopy = 1;
}

EndOfProgram()
{
}
