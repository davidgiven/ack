/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<string.h>

void *
memchr(const void *s, int c, register size_t n)
{
	register const unsigned char *s1 = s;
	unsigned char c1 = (unsigned char) c;
	
	while (n > 0) {
		n--;
		if (*s1++ == c1)
			return (void *) (s1 - 1);
	}
	return NULL;
}
