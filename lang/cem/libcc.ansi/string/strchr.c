/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<string.h>

char *
strchr(register const char *s, int c)
{
	register char c1 = (char) c;

	do {
		if (*s == c1)
			return s;
	} while (*s++ != '\0');

	return (char *)NULL;
}
