/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<string.h>

void *
memmove(register void *s1, register const void *s2, register size_t n)
{
	void *ret = s1;

	if (s2 <= s1 && s2 + (n-1) >= s1) {
		/* overlap, copy backwards */
		s1 += n;
		s2 += n;
		while (n > 0) {
			n--;
			*--s1 = *--s2;
		}
	} else
		while (n > 0) {
			n--;
			*s1++ = *s2++;
		}
	return ret;
}
