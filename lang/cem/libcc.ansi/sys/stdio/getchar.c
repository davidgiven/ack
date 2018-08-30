/*
 * getchar.c - read a character from the standard input stream
 */
/* $Id$ */

#include <stdio.h>

#if ACKCONF_WANT_STDIO

int(getchar)(void)
{
	return getchar();
}

#endif
