/* $Id$ */
#include <stdio.h>

char *fgets(str, n, file)
char *str;
unsigned n;
FILE *file;
{
	register int ch;
	register char *ptr;

	ptr = str;
	while ( --n > 0 && (ch = getc(file)) != EOF){
		*ptr++ = ch;
		if ( ch == '\n')
			break;
	}
	if (ch == EOF && ptr==str)
		return(NULL);
	*ptr = '\0';
	return(str);
}
