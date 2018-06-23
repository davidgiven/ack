/*
 * ferror .c - test if an error on a stream occurred
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int(ferror)(FILE* stream)
{
	return ferror(stream);
}

#endif
