/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<string.h>

void *
memcpy(register void *s1, register const void *s2, register size_t n)
{
	void *ret = s1;

	while (n > 0) {
		n--;
		*s1++ = *s2++;
	}
	return ret;
}
