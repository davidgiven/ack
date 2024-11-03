/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <string.h>
#include <locale.h>

int strcoll(const char* s1, const char* s2)
{
	while (*s1 == *s2++)
	{
		if (*s1++ == '\0')
		{
			return 0;
		}
	}
	return *s1 - *--s2;
}
