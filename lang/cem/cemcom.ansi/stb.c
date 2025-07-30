/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stddef.h>
#include <stdbool.h>

/* library routine for copying structs */
void __stb(int n, char* f, char* t)
{
	if (n > 0)
		do
			*t++ = *f++;
		while (--n);
}
