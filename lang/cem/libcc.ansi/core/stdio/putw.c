/* 
 * putw - write an word on a stream
 */
/* $Id$ */

#include <stdio.h>

int putw(int w, FILE* stream)
{
	int cnt = sizeof(int);
	char* p = (char*)&w;

	while (cnt--)
	{
		putc(*p++, stream);
	}
	if (ferror(stream))
		return EOF;
	return w;
}
