/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	clear - clear a block of memory, and try to do it fast.
*/

#include "alloc.h"

/* instead of Calloc: */

#if __STDC__
void
#endif
clear(ptr, n)
	register char *ptr;
	register unsigned int n;
{
	register long *q = (long *) ptr;

	while (n >= 8*sizeof (long))	{
			/* high-speed clear loop */
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		n -= 8*sizeof (long);
	}
	while (n >= sizeof (long))	{
			/* high-speed clear loop */
		*q++ = 0;
		n -= sizeof (long);
	}
	ptr = (char *) q;
	while (n--) *ptr++ = '\0';
}
