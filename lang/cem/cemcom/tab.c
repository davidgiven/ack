/* $Header$ */
/*	@cc tab.c -o $INSTALLDIR/tab@
	tab - table generator 

	Author: Erik Baalbergen (..tjalk!erikb)
*/

#include <stdio.h> 

#define MAXTAB	10000
#define MAXBUF	10000
#define COMCOM	'-'
#define FILECOM	'%'

int InputForm = 'c';
char OutputForm[MAXBUF] = "%s,\n";
int TabSize = 257;
char *Table[MAXTAB];
char *ProgCall;

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
			process(*argv++, InputForm);
		}
	}
}

char *
Salloc(s)
	char *s;
{
	extern char *malloc(), *strcpy();
	char *ns = malloc((unsigned int)strlen(s) + 1);

	if (ns) {
		strcpy(ns, s);
	}
	return ns;
}

option(str)
	char *str;
{
	/*	note that *str indicates the source of the option:
		either COMCOM (from command line) or FILECOM (from a file).
	*/
	extern char *sprintf();
	
	switch (*++str) {

	case ' ':	/* command */
	case '\t':
	case '\0':
		break;
	case 'I':
		InputForm = *++str;
		break;
	case 'f':
		if (*++str == '\0') {
			fprintf(stderr, "%s: -f: name expected\n", ProgCall);
			exit(1);
		}
		DoFile(str);
		break;
	case 'F':
		sprintf(OutputForm, "%s\n", ++str);
		break;
	case 'T':
		printf("%s\n", ++str);
		break;
	case 'p':
		PrintTable();
		break;
	case 'C':
		ClearTable();
		break;
	case 'S':
	{
		register i = stoi(++str);

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

ClearTable()
{
	register i;

	for (i = 0; i < MAXTAB; i++) {
		Table[i] = 0;
	}
}

PrintTable()
{
	register i;

	for (i = 0; i < TabSize; i++) {
		if (Table[i]) {
			printf(OutputForm, Table[i]);
		}
		else {
			printf(OutputForm, "0");
		}
	}
}

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

	while (*str)	{
		if (*str == '\\')	{
			ch = quoted(&str);
		}
		else	{
			ch = *str++;
		}
		if (*str == '-')	{
			if (*++str == '\\')	{
				ch2 = quoted(&str);
			}
			else	{
				if (ch2 = *str++);
				else str--;
			}
			if (ch > ch2)	{
				fprintf(stderr, "%s: bad range\n", ProgCall);
				return 0;
			}
			if (ch >= 0 && ch2 <= 255)
				while (ch <= ch2)
					Table[ch++] = Salloc(Name);
		}
		else	{
			if (ch >= 0 && ch <= 255)
				Table[ch] = Salloc(Name);
		}
	}
	return 1;
}
			
int
quoted(pstr)
	char **pstr;
{
	register int ch;
	register int i;
	register char *str = *pstr;

	if ((*++str >= '0') && (*str <= '9'))	{
		ch = 0;
		for (i = 0; i < 3; i++)	{
			ch = 8 * ch + *str - '0';
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
		default :
			ch = *str;
		}
	}
	*pstr = str;
	return ch & 0377;
}

int
stoi(str)
	char *str;
{
	register i = 0;

	while (*str >= '0' && *str <= '9') {
		i = i * 10 + *str++ - '0';
	}
	return i;
}

char *
getline(s, n, fp)
	char *s;
	FILE *fp;
{
	register c = getc(fp);
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
			process(text, InputForm);
		}
	}
}
