/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <string.h>

void* memset(void* s, int c, size_t n)
{
	char* s1 = s;

	if (n > 0)
	{
		n++;
		while (--n > 0)
		{
			*s1++ = c;
		}
	}
	return s;
}
