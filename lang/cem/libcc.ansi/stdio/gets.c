/*
 * gets.c - read a line from a stream
 */
/* $Header$ */

#include	<stdio.h>

char *
gets(char *s)
{
	register int ch;
	register char *ptr;

	ptr = s;
	while ((ch = getc(stdin)) != EOF && ch != '\n')
		*ptr++ = ch;

	if (ch == EOF && ptr==s)
		return (char *)NULL;

	*ptr = '\0';
	return s;
}
