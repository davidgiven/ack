/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<string.h>

int
memcmp(register const void *s1, register const void *s2, size_t n)
{
	while (n > 0) {
		n--;
		if (*s1++ != *s2++)
			return *--s1 - *--s2;
	}
	return 0;
}
