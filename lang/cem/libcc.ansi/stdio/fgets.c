/*
 * fgets.c - get a string from a file
 */
/* $Header$ */

#include	<stdio.h>

char *
fgets(char *s, int n, FILE *stream)
{
	register int ch;
	register char *ptr;

	ptr = s;
	while (--n > 0 && (ch = getc(stream)) != EOF) {
		*ptr++ = ch;
		if ( ch == '\n')
			break;
	}
	if (ch == EOF && ptr == s)
		return (char *)NULL;
	*ptr = '\0';
	return s;
}
