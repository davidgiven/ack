/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>
#include	<limits.h>

int
wctomb(char *s, wchar_t wchar)
{
	if (s != (char *)NULL) {
		*s = wchar;
		return MB_LEN_MAX;
	} else
		return 0;
}

