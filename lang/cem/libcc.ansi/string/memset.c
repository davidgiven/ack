/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<string.h>

void *
memset(void *s, int c, register size_t n)
{
	register void *s1 = s;

	while (n > 0) {
		n--;
		*s1++ = c;
	}
	return s;
}
