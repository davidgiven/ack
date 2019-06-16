/*
 * putchar.c - print (or buffer) a character on the standard output stream
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int(putchar)(int c)
{
	return putchar(c);
}

#endif
