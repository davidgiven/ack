/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>

#define ALIGN(sz)	(((sz)+(sizeof(long)-1)/sizeof(long))*sizeof(long))

void *
calloc(size_t nelem, size_t elsize)
{
	register void *p;
	register long *q;
	unsigned int size = ALIGN(nelem * elsize);

	if ((p = malloc(size)) == (void *)NULL)
		return (void *)NULL;
	/* "The space is intialized to all bits zero" */
	q = (long *) (p + size);
	while ((void *) q > p) *--q = 0;
	return p;
}
