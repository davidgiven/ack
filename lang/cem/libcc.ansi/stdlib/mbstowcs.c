/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>
#include	<locale.h>
#include	<limits.h>

size_t
mbstowcs(register wchar_t *pwcs, register const char *s, size_t n)
{
	register int i = 0;

	while (i < n) {
		i++;
		*pwcs++ = *s++;
		if (*s == '\0')
			return i;
	}
	return n;
}

