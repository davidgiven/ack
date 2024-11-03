/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <string.h>

void* memcpy(void* s1, const void* s2, size_t n)
{
	char* p1 = s1;
	const char* p2 = s2;

	if (n)
	{
		n++;
		while (--n > 0)
		{
			*p1++ = *p2++;
		}
	}
	return s1;
}
