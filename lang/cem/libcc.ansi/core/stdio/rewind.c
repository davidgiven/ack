/*
 * rewind.c - set the file position indicator of a stream to the start
 */
/* $Id$ */

#include <stdio.h>

void rewind(FILE* stream)
{
	(void)fseek(stream, 0L, SEEK_SET);
	clearerr(stream);
}
