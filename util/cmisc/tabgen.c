/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
	chtabgen - character table generator

	Author: Erik Baalbergen (..tjalk!erikb)
	Many mods by Ceriel Jacobs
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef NORCSID
static char *RcsId = "$Id$";
#endif

#define MAXBUF	256
#define MAXTAB 10000
#define COMCOM	'-'
#define FILECOM	'%'

int InputForm = 'c';	/* default input format (and, currently, only one) */
char OutputForm[MAXBUF] = "%s,\n";
			/* format for spitting out a string */
char *Table[MAXTAB];
char *ProgCall;		/* callname of this program */
int TabSize = 128;	/* default size of generated table */
char *InitialValue;	/* initial value of all table entries */

main(argc, argv)
	char *argv[];
{

	ProgCall = *argv++;
	argc--;
	while (argc-- > 0) {
		if (**argv == COMCOM) {
			option(*argv++);
		}
		else {
			if (! process(*argv++, InputForm)) {
				exit(1);
			}
		}
	}
	exit(0);
	/*NOTREACHED*/
}

char *
Salloc(s)
	char *s;
{
	char *ns = malloc((unsigned)strlen(s) + 1);

	if (ns) {
		strcpy(ns, s);
	}
	else {
		fprintf(stderr, "%s: out of memory\n", ProgCall);
		exit(1);
	}
	return ns;
}

option(str)
	char *str;
{
	/*	note that *str indicates the source of the option:
		either COMCOM (from command line) or FILECOM (from a file).
	*/
	switch (*++str) {

	case ' ':	/* command */
	case '\t':
	case '\0':
		break;
	case 'I':	/* for future extension */
		InputForm = *++str;
		break;
	case 'f':	/* input from file ... */
		if (*++str == '\0') {
			fprintf(stderr, "%s: -f: name expected\n", ProgCall);
			exit(1);
		}
		DoFile(str);
		break;
	case 'F':	/* new output format string */
		sprintf(OutputForm, "%s\n", ++str);
		break;
	case 'T':	/* insert text literally */
		printf("%s\n", ++str);
		break;
	case 'p':	/* print table */
		PrintTable();
		break;
	case 'C':	/* clear table */
		InitTable((char *)0);
		break;
	case 'i':	/* initialize table with given value */
		if (*++str == '\0') {
			InitTable((char *)0);
		}
		else	InitTable(str);
		break;
	case 'S':
	{
		int i = atoi(++str);

		if (i <= 0 || i > MAXTAB) {
			fprintf(stderr, "%s: size would exceed maximum\n",
				ProgCall);
		}
		else {
			TabSize = i;
		}
		break;
	}
	default:
		fprintf(stderr, "%s: bad option -%s\n", ProgCall, str);
	}
}

InitTable(ival)
	char *ival;
{
	int i;

	for (i = 0; i < TabSize; i++) {
		Table[i] = 0;
	}
	InitialValue = 0;
	if (ival) {
		InitialValue = Salloc(ival);
	}
}

PrintTable()
{
	int i;

	for (i = 0; i < TabSize; i++) {
		if (Table[i]) {
			printf(OutputForm, Table[i]);
		}
		else if (InitialValue) {
			printf(OutputForm, InitialValue);
		}
		else {
			printf(OutputForm, "0");
		}
	}
}

int
process(str, format)
	char *str;
{
	char *cstr = str;
	char *Name = cstr;	/* overwrite original string!	*/

	/* strip of the entry name
	*/
	while (*str && *str != ':') {
		if (*str == '\\') {
			++str;
		}
		*cstr++ = *str++;
	}

	if (*str != ':') {
		fprintf(stderr, "%s: bad specification: \"%s\", ignored\n",
			ProgCall, Name);
		return 0;
	}
	*cstr = '\0';
	str++;

	switch (format) {

	case 'c':
		return c_proc(str, Name);
	default:
		fprintf(stderr, "%s: bad input format\n", ProgCall);
	}
	return 0;
}

c_proc(str, Name)
	char *str;
	char *Name;
{
	int ch, ch2;
	int quoted();
	char *name = Salloc(Name);

	while (*str)	{
		if (*str == '\\')	{
			ch = quoted(&str);
		}
		else	{
			ch = *str++ & 0377;
		}
		if (*str == '-')	{
			if (*++str == '\\')	{
				ch2 = quoted(&str);
			}
			else	{
				if (ch2 = (*str++ & 0377));
				else str--;
			}
			if (ch > ch2) {
				fprintf(stderr, "%s: bad range\n", ProgCall);
				return 0;
			}
			while (ch <= ch2) {
				if (! setval(ch, name)) return 0;
				ch++;
			}
		}
		else	{
			if (! setval(ch, name)) return 0;
		}
	}
	return 1;
}

int
setval(ch, nm)
	char *nm;
{
	char **p = &Table[ch];

	if (ch < 0 || ch >= TabSize) {
		fprintf(stderr, "Illegal index: %d\n", ch);
		return 0;
	}
	if (*(p = &Table[ch])) {
		fprintf(stderr, "Warning: redefinition of index %d\n", ch);
	}
	*p = nm;
	return 1;
}

int
quoted(pstr)
	char **pstr;
{
	int ch;
	int i;
	char *str = *pstr;

	if ((*++str >= '0') && (*str <= '9'))	{
		ch = 0;
		for (i = 0; i < 3; i++)	{
			ch = 8 * ch + (*str - '0');
			if (*++str < '0' || *str > '9')
				break;
		}
	}
	else	{
		switch (*str++)	{
		case 'n':
			ch = '\n';
			break;
		case 't':
			ch = '\t';
			break;
		case 'b':
			ch = '\b';
			break;
		case 'r':
			ch = '\r';
			break;
		case 'f':
			ch = '\f';
			break;
		case 'v':
			ch = 013;
			break;
		default :
			ch = *(str - 1);
			break;
		}
	}
	*pstr = str;
	return ch & 0377;
}

char *
getline(s, n, fp)
	char *s;
	FILE *fp;
{
	int c = getc(fp);
	char *str = s;

	while (n--) {
		if (c == EOF) {
			return NULL;
		}
		else
		if (c == '\n') {
			*str++ = '\0';
			return s;
		}
		*str++ = c;
		c = getc(fp);
	}
	s[n - 1] = '\0';
	return s;
}

#define BUFSIZE 1024

DoFile(name)
	char *name;
{
	char text[BUFSIZE];
	FILE *fp;

	if ((fp = fopen(name, "r")) == NULL) {
		fprintf(stderr, "%s: cannot read file %s\n", ProgCall, name);
		exit(1);
	}
	while (getline(text, BUFSIZE, fp) != NULL) {
		if (text[0] == FILECOM) {
			option(text);
		}
		else {
			if (! process(text, InputForm)) {
				exit(1);
			}
		}
	}
}
