/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	Print IDentifiers occurring in C programs outside comment, strings
	and character constants.
	Flags:
	-l<num>		: print identifiers of <num> or more characters
			  (default <num> = 8)

	Author: Erik Baalbergen
	Date: Oct 23, 1985
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern char *ProgName;

#ifndef DEF_LENGTH
#define DEF_LENGTH	8
#endif

int maxlen = DEF_LENGTH;


void InsertId(char *);
char *Malloc(unsigned int);
char *Salloc(char *);
int EnHash(char*);
void EndOfProgram(void);
void DoOption(char*);
void CheckId(char *, int);




void BeginOfProgram(void) {}

void DoOption(char* str)
{
	switch (str[1]) {

	case 'l':
		if ((maxlen = atoi(&str[2])) <= 0) {
			fprintf(stderr, "%s: option \"-l%s\" ignored\n",
				ProgName, &str[2]);
			maxlen = DEF_LENGTH;
		}
		break;
	
	default:
		fprintf(stderr, "%s: bad option \"%s\"\n", ProgName, str);
		break;
	}
}

void CheckId(char *id, int len)
{
	if (len >= maxlen) {
		InsertId(id);
	}
}

#define HASHSIZE 257

struct idf {
	char *id_name;
	struct idf *id_next;
};

struct idf *hash_tab[HASHSIZE];

void InsertId(char *id)
{
	int hash_val = EnHash(id);
	register struct idf *idp = hash_tab[hash_val];
	register struct idf *p = 0;

	while (idp && strcmp(idp->id_name, id)) {
		p = idp;
		idp = idp->id_next;
	}

	if (idp == 0) {
		idp = (struct idf *) Malloc(sizeof(struct idf));
		idp->id_next = 0;
		if (!p) hash_tab[hash_val] = idp;
		else p->id_next = idp;
		idp->id_name = Salloc(id);
	}
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

	if (str == 0)
		str = "";

	return strcpy(Malloc((unsigned)strlen(str) + 1), str);
}

int EnHash(char *id)
{
	register unsigned hash_val = 0;
	register int n = maxlen;

	while (n-- && *id)
		hash_val = 31 * hash_val + *id++;

	return hash_val % (unsigned) HASHSIZE;
}

void EndOfProgram(void)
{
	register struct idf *idp;
	register int i;

	for (i = 0; i < HASHSIZE; i++) {
		for (idp = hash_tab[i]; idp; idp = idp->id_next) {
			printf("%s\n", idp->id_name);
		}
	}
}
