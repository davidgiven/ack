/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>

size_t
mbstowcs(wchar_t* pwcs, const char* s, size_t n)
{
	int i = n;

	while (--i >= 0)
	{
		if (!(*pwcs++ = *s++))
			return n - i - 1;
	}
	return n - i;
}
