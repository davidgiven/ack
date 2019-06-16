/*
 * fgetc - get an unsigned character and return it as an int
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int fgetc(FILE* stream)
{
	return getc(stream);
}

#endif
