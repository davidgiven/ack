/*
 * File:  -  new.c
 *
 * new() built in standard procedure in Pascal (6.6.5.3)
 *
 * Re-implementation of storage allocator for Ack Pascal compiler
 * under Linux, and other UNIX-like systems.
 *
 * Written by Erik Backerud, 2010-10-01
 *
 * Original copyright and author info below:
 */
/* $Id$ */
/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/* Author: J.W. Stevenson */
#include <em_abs.h>
#include <pc_err.h>

#define assert(x)	/* nothing */
#define	UNDEF		0x8000
#define NALLOC		(1024)		/* request this many units from OS */


/*
 * use a singly linked list of free blocks.
 */
struct adm {
	struct adm	*next;
	int		size;
};

extern struct adm	*freep;

extern void _trp(int);			/* called on error */

extern void _dis(int, struct adm **);


/*
 * Helper function to request 'nu' units of memory from the OS.
 * A storage unit is sizeof(struct adm). Typically 8 bytes
 * on a 32-bit machine like i386 etc.
 */
static struct adm *
morecore(unsigned nu)
{
    char *cp, *sbrk(int);
    struct adm *up;

    if (nu < NALLOC)
	nu = NALLOC;
    cp = sbrk(nu * sizeof(struct adm));
    if (cp == (char *) -1) /* no space at all */
	return 0;
    up = (struct adm*) cp;
    up->size = nu;
    up = up + 1;
    _dis((nu - 1) * sizeof(struct adm), &up);
    return freep;
}   /* morecore */

/*
 * Dispose
 * Called with two arguments:
 * n the size of the block to be freed, in bytes,
 * pp address of pointer to data.
 */
void
_new(int n, struct adm **pp)
{
    int nunits;    /* the unit of storage is sizeof(struct adm) */
    struct adm *p,*q;

    /* round up size of request */
    nunits  = (n + sizeof(struct adm) - 1) / sizeof(struct adm) + 1;

    q = 0;
    for (p = freep; ; p = p->next) {
	if (p == 0) {
	    p = morecore(nunits);
	    if (p == 0)
		_trp(EHEAP);
	    q = 0;
	}
	if (p->size >= nunits) {
	    if (p->size == nunits) {	/* exact fit */
		if (q == 0) {	/* first element on free list. */
		    freep = p->next;
		} else {
		    q->next = p->next;
		}
	    } else {		/* allocate tail end */
		q = p;
		q->size = q->size - nunits;
		p = q + q->size;
		p->next = 0;
		p->size = nunits;
	    }
	    break;
	}
	q = p;
    }
    *pp = p + 1;
}   /* _new */
