/*
 * clearerr.c - clear error and end-of-file indicators of a stream
 */
/* $Header$ */

#include	<stdio.h>

void
clearerr(FILE *stream)
{
	stream->_flags &= ~(_IOERR|_IOEOF);
}
