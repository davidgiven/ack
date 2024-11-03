/*
 * fgets.c - get a string from a file
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

char* fgets(char* s, int n, FILE* stream)
{
	int ch;
	char* ptr;

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
