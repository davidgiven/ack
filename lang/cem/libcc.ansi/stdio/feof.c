/*
 * feof.c - test if eof on a stream occurred
 */
/* $Header$ */

#include	<stdio.h>

#undef	feof

int
feof(FILE *stream)
{
	return (stream->_flags & _IOEOF) != 0;
}
