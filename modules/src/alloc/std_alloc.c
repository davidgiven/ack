/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	std_alloc - get a structure from a free list. If no structures left,
		create new ones.
		The counterpart, st_free, is a macro, defined in alloc.h
	This is a counting version of st_alloc.
*/

#if __STDC__
#include <stdlib.h>
#else
extern char *malloc();
#endif

#include	"alloc.h"

char *std_alloc(char **phead, register unsigned int size, int count, int *pcnt)
{
	register char *p;
	register long *q;
	char *retval;

	if (*phead == 0)	{
		while (count >= 1 && (p = malloc(size * count)) == 0) {
			count >>= 1;
		}
		if (p == 0) {
			No_Mem();
		}
		*pcnt += count;
		((_PALLOC_) p)->_A_next = 0;
		while (--count) {
			p += size;
			((_PALLOC_) p)->_A_next = (_PALLOC_) (p - size);
		}
		*phead = p;
	}
	else p = *phead;
	*phead = (char *) (((_PALLOC_) p)->_A_next);
	retval = p;
	q = (long *) p;
	while (size >= 8*sizeof(long)) {
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		*q++ = 0;
		size -= 8*sizeof(long);
	}
	while (size >= sizeof(long)) {
		*q++ = 0;
		size -= sizeof(long);
	}
	p = (char *) q;

	while (size--) *p++ = 0;
	return retval;
}
