/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	st_alloc - get a structure from a free list. If no structures left,
		create new ones. The structures for which this works are
		supposed to have as their first tag the string "next", which
		should be a pointer type.
		The counterpart, st_free, is a macro, defined in alloc.h
*/

#include	"alloc.h"

struct xxx	{
	char *next;
};

char *
st_alloc(phead, size, count)
	char **phead;
	register unsigned int size;
{
	register char *p;
	char *retval;

	if (*phead == 0)	{

		p = Malloc(size * count);
		((struct xxx *) p)->next = 0;
		while (--count) {
			p += size;
			((struct xxx *) p)->next = p - size;
		}
		*phead = p;
	}
	else	p = *phead;
	*phead = ((struct xxx *)p)->next;
	retval = p;
	if (size >= sizeof(long)) {
		register long *q = (long *) p;
		do {
			*q++ = 0;
			size -= sizeof(long);
		} while (size >= sizeof(long));

		p = (char *) q;
	}

	while (size--) *p++ = 0;
	return retval;
}
