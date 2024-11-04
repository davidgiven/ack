/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <string.h>

size_t
strxfrm(char* s1, const char* save, size_t n)
{
	const char* s2 = save;

	while (*s2)
	{
		if (n > 1)
		{
			n--;
			*s1++ = *s2++;
		}
		else
			s2++;
	}
	if (n > 0)
		*s1++ = '\0';
	return s2 - save;
}
