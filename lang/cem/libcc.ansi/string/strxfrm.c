/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<string.h>

size_t
strxfrm(register char *s1, register const char *s2, register size_t n)
{
	const char *save = s2;

	while (*s2) {
		if (n > 1) {
			n--;
			*s1++ = *s2++;
		} else
			s2++;
	}
	if (n >= 1)
		*s1++ = '\0';
	return s2 - save;
}
