#include	<stdio.h>
#if	__STDC__ == 1
#include	<stdlib.h>
#include	<string.h>
#else
#define	EXIT_SUCCESS	0
#define	EXIT_FAILURE	1
#endif

#define	UCHAR_MAX	256

char *functab[] = {
	"isalnum",
	"isalpha",
	"iscntrl",
	"isdigit",
	"isgraph",
	"islower",
	"isprint",
	"ispunct",
	"isspace",
	"isupper",
	"isxdigit",
	"isascii",
	"toupper",
	"tolower",
	 NULL,
};

char buf[100];

int
main()
{
	register char **name;
	register int i;
	FILE *file;

	name = functab;
	while (*name) {
		strcpy(buf, *name);
		strcat(buf, ".c");
		if (!(file = fopen(buf,"w"))) exit(EXIT_FAILURE);
		fprintf(file,"int (%s)(int c) {\n", *name);
		fprintf(file,"\treturn %s(c);\n", *name);
		fprintf(file,"}\n");
		fclose(file);
		name++;
	}
	exit(EXIT_SUCCESS);
}
