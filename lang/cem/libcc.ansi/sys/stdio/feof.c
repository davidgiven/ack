/*
 * feof.c - test if eof on a stream occurred
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int(feof)(FILE* stream)
{
	return feof(stream);
}

#endif
