/*
 * File:  -  dis.c
 *
 * dispose() built in standard procedure in Pascal (6.6.5.3)
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

#include	<pc_err.h>

#define assert()	/* nothing */

/*
 * use a singly linked list of free blocks.
 */
struct adm {
	struct adm	*next;
	int		size;
};

struct adm *freep = 0;			/* first element on free list */

extern void _trp(int);

/*
 * Dispose
 * Called with two arguments:
 * n the size of the block to be freed, in bytes,
 * pp address of pointer to data.
 */
void
_dis(int n, struct adm **pp)
{
    struct adm *block;	/* the block of data being freed (inc. header) */
    struct adm *p, *q;

    if (*pp == 0) {
	_trp(EFREE);
    }
    block = *pp - 1;
    if (freep == 0) {
	freep = block;
	block->next = 0;
    } else {
	q = 0;	/* trail one behind */
	for (p = freep; p < block; p = p->next) {
	    if (p == 0) {	/* We reached the end of the free list. */
		break;
	    }
	    q = p;
	    /* check if block is contained in the free block p */
	    if (p+p->size > block) {
		_trp(EFREE);
	    }
	}
	if (p == block) {	/* this block already freed */
	  _trp(EFREE);
	}
	if (q == 0) {	/* block is first */
	    freep = block;
	    block->next = p;
	} else {
	    q->next = block;
	}
	block->next = p;
	/* merge with successor on free list? */
	if (block + block->size == p) {
	    block->size = block->size + p->size;
	    block->next = p->next;
	}
	/* merge with preceding block on free list? */
	if (q != 0 && q+q->size == block) {
	    q->size = q->size + block->size;
	    q->next = block->next;
	}
    }
}   /* _dis */
