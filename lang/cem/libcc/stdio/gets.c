/* $Id$ */
#include <stdio.h>

char *gets(str)
char *str;
{
	int ch;
	char *ptr;

	ptr = str;
	while ((ch = getc(stdin)) != EOF && ch != '\n')
		*ptr++ = ch;

	if (ch == EOF && ptr==str)
		return(NULL);
	*ptr = '\0';
	return(str);
}
