/*
 * ferror .c - test if an error on a stream occurred
 */
/* $Header$ */

#include	<stdio.h>

#undef	ferror

int
ferror(FILE *stream)
{
	return (stream->_flags & _IOERR) != 0;
}
