/*
 * setbuf.c - control buffering of a stream
 */
/* $Header$ */

#include	<stdio.h>
#include	"loc_incl.h"

void
setbuf(register FILE *stream, char *buf)
{
	int mode;

	if (buf) mode = _IOFBF;
	else mode = _IONBF;

	(void) setvbuf(stream, buf, mode, (size_t) BUFSIZ);
}
