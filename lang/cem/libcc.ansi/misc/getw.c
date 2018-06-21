/*
 * getw - read a word from a stream
 */
/* $Id$ */

#include <stdio.h>

int getw(register FILE* stream)
{
	register int cnt = sizeof(int);
	int w;
	register char* p = (char*)&w;

	while (cnt--)
	{
		*p++ = getc(stream);
	}
	if (feof(stream) || ferror(stream))
		return EOF;
	return w;
}
