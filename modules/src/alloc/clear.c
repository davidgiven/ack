/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	clear - clear a block of memory, and try to do it fast.
*/

#include <assert.h>
#include "in_all.h"

/* instead of Calloc: */
EXPORT
clear(ptr, n)
	register char *ptr;
	register int n;
{
	register long *q = (long *) ptr;

	assert((long)q % sizeof (long) == 0);
	while (n >= sizeof (long))	{
			/* high-speed clear loop */
		*q++ = 0;
		n -= sizeof (long);
	}
	ptr = (char *) q;
	while (n--) *ptr++ = '\0';
}
