/*
 * fgets.c - get a string from a file
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

char* fgets(char* s, register int n, register FILE* stream)
{
	register int ch;
	register char* ptr;

	ptr = s;
	while (--n > 0 && (ch = getc(stream)) != EOF)
	{
		*ptr++ = ch;
		if (ch == '\n')
			break;
	}
	if (ch == EOF)
	{
		if (feof(stream))
		{
			if (ptr == s)
				return NULL;
		}
		else
			return NULL;
	}
	*ptr = '\0';
	return s;
}

#endif
