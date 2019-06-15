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
#include <string.h>
#include <stdlib.h>

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

char *Malloc(unsigned int);
char *Salloc(char *);
int EnHash(char*);
void EndOfProgram(void);
void DoOption(char*);
void CheckId(char *, int);
void DoMacro(char *);
void GetMacros(char *);
void InsertMacro(char *, char *);
struct idf *FindId(char *);



extern char *ProgName;

void DoOption(char* str)
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
void CheckId(char *id, int len)
{
	struct idf *idp = FindId(id);

	if (idp) {
		printf("%s", idp->id_text);
	}
	else {
		printf("%s", id);
	}
}

void DoMacro(char *str)
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

void GetMacros(char *fn)
{
	FILE *fp;
	register int c;
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

void InsertMacro(char *id, char *text)
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

char *Malloc(unsigned int n)
{
	char *mem;

	if ((mem = malloc(n)) == 0) {
		fprintf(stderr, "%s: out of memory\n", ProgName);
		exit(1);
	}
	return mem;
}

char *Salloc(char *str)
{
	if (str == 0) {
		str = "";
	}
	return strcpy(Malloc((unsigned)strlen(str) + 1), str);
}

struct idf *FindId(char *id)
{
	register int hash_val = EnHash(id);
	register struct idf *idp = hash_tab[hash_val];

	while (idp) {
		if (strcmp(idp->id_name, id) == 0) {
			return idp;
		}
		idp = idp->id_next;
	}
	return 0;
}

int EnHash(char *id)
{
	register unsigned hash_val = 0;

	while (*id) {
		hash_val = 31 * hash_val + *id++;
	}

	return hash_val % (unsigned) HASHSIZE;
}

extern int GCcopy;

void BeginOfProgram(void)
{
	GCcopy = 1;
}

void EndOfProgram(void)
{
}
