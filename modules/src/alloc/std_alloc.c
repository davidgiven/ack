/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	st_alloc - get a structure from a free list. If no structures left,
		create new ones.
		The counterpart, st_free, is a macro, defined in alloc.h
*/

#include	"alloc.h"

char *
std_alloc(phead, size, count, pcnt)
	char **phead;
	register unsigned int size;
	int *pcnt;
{
	register char *p;

	if (*phead == 0)	{

		p = Malloc(size * count);
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
	p += size;
	while (size--) *--p = 0;
	return p;
}
