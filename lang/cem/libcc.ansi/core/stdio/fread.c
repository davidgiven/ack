/*
 * fread.c - read a number of members into an array
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

size_t
fread(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	char* cp = ptr;
	int c;
	size_t ndone = 0;
	size_t s;

	if (size)
		while (ndone < nmemb)
		{
			s = size;
			do
			{
				if ((c = getc(stream)) != EOF)
					*cp++ = c;
				else
					return ndone;
			} while (--s);
			ndone++;
		}

	return ndone;
}

#endif
