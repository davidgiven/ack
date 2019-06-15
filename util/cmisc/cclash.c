/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
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
	Revised: Tue Nov 11 13:32:31 MET 1986
		by Ceriel Jacobs,
		to produce lists for "cid" or preprocessor in
		alphabetical order.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEF_LENGTH	8

struct idf {
	struct idf *id_next;
	char *id_name;
	struct idf *id_same;
	char id_key;
};

#define ACT_LISTONLY	0
#define ACT_MAPFILE	1
#define ACT_CID		2

int maxlen = DEF_LENGTH;
int action = ACT_LISTONLY;

extern char *ProgName;

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


void InsertId(char *,int);
char *Malloc(unsigned int);
char *Salloc(char *);
int EnHash(char*);
void EndOfProgram(void);
void DoOption(char*);
void CheckId(char *, int);
void saveline(register struct idf *);
void mapline(char *);



struct idf *maplist = 0;

void DefineKeys(void)
{
	register char **pkey = &keywords[0];
	register char *id;

	while ((id = *pkey++))
		if (strlen(id) >= maxlen)
			InsertId(id, 1);
}

void DoOption(char *str)
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

void InsertId(char *id, int key)
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

	p->id_key = key;
}

char *Malloc(unsigned n)
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

void BeginOfProgram(void) { DefineKeys(); }

void EndOfProgram(void)
{
	register int i;
	register struct idf *idp, *p;

	for (i = 0; i < HASHSIZE; i++) {
		for (idp = hash_tab[i]; idp; idp = idp->id_next) {
			if (idp->id_same == 0)
				continue;

			switch (action) {
				register int n;

			case ACT_LISTONLY:
				n = 0;
				if (idp->id_key == 0) {
					printf("%s",idp->id_name);
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
				for (p = idp->id_same; p;) {
					register struct idf *q = p->id_same;

					if (p->id_key == 0)
						saveline(p);
					p = q;
				}
				if (idp->id_key == 0)
					saveline(idp);
				break;
			}
		}
	}
	switch(action) {
	case ACT_CID:
	case ACT_MAPFILE:
		for (idp = maplist; idp; idp = idp->id_same) {
			mapline(idp->id_name);
		}
	}
}

void saveline(register struct idf *p)
{
	register struct idf *idp = maplist, *idp1 = 0;

	while (idp && strcmp(idp->id_name, p->id_name) < 0) {
		idp1 = idp;
		idp = idp->id_same;
	}
	p->id_same = idp;
	if (idp1 == 0) {
		maplist = p;
	}
	else {
		idp1->id_same = p;
	}
}

void mapline(char *nm)
{
	static int map_count = 0;

	switch (action) {

	case ACT_MAPFILE:
		printf("#define %s _%d_%s\n", nm, ++map_count, nm);
		break;
	
	case ACT_CID:
		printf("%s=_%d_%s\n", nm, ++map_count, nm);
		break;
	}
}

void CheckId(char *id, int s)
{
	if (s >= maxlen)
		InsertId(id, 0);
}
