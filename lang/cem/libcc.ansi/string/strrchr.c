/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<string.h>

char *
strrchr(register const char *s, int c)
{
	register const char *result;
	register char c1 = (char) c;

	result = (char *)NULL;
	do {
		if (*s == c1)
			result = s;
	} while (*s++);

	return (char *)result;
}
