/*
 * clearerr.c - clear error and end-of-file indicators of a stream
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

void(clearerr)(FILE* stream)
{
	clearerr(stream);
}

#endif
