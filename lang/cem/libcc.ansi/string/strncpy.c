/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<string.h>

char *
strncpy(register char *s1, register const char *s2, size_t n)
{
	char *ret = s1;

	while (*s2 && n > 0) {
		n--;
		*s1++ = *s2++;
	}
	while (n > 0) {
		n--;
		*s1++ = '\0';
	}
	return ret;
}
