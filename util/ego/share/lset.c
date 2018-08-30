/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  L O N G   S E T S
 *
 *  L S E T . C
 */


#include <stdlib.h>
#include "types.h"
#include "lset.h"
#include "alloc.h"
#include "debug.h"


/* A 'long' set is represented as a linear list of 'elemholder'
 * records. Every such record contains a pointer to an element
 * of the set and to the next elemholder. An empty set is
 * represented as a null pointer.
 * An element of a long set must be of some pointer type or,
 * in any case, must have the size of a pointer. Note that
 * the strict typing rules are not obeyed here.
 * This package implements the usual operations on sets.
 * The name of every operation is preceeded by a 'L' to
 * distinguish it from the operation on 'compact' (bitvector)
 * sets with a similar name.
 */


lset Lempty_set(void)
{
	return ((lset) 0);
}


bool Lis_elem(Lelem_t x, lset s)
{

	/* Search the list to see if x is an element of s */
	while (s != (elem_p) 0) {
		if (s->e_elem == x) {
			return TRUE;
		}
		s = s->e_next;
	}
	return FALSE;
}


void Ladd(Lelem_t x, lset *s_p)
{
	/* add x to a set. Note that the set is given as in-out
	 * parameter, because it may be changed.
	 */

	elem_p t;

	if (!Lis_elem(x,*s_p)) {
		t = newelem();  /* allocate a new elemholder */
		t->e_elem = x;
		t->e_next = *s_p;  /* insert it at the head of the list */
		*s_p = t;
	}
}


void Lremove(Lelem_t x, lset *s_p)
{
	/* Remove x from a set. If x was not an element of
	 * the set, nothing happens.
	 */

	register elem_p *epp, ep;
	lset s;

	s = *s_p;
	epp = &s;
	while ((ep = *epp) != (elem_p) 0) {
		if (ep->e_elem == x) {
			*epp = ep->e_next;
			oldelem(ep);
			break;
		} else {
			epp = &ep->e_next;
		}
	}
	*s_p = s;
}


/* The operations first, next and elem can be used to iterate
 * over a set. For example:
 *	for (i = Lfirst(s); i != (Lindex) 0; i = Lnext(i,s) {
 *		x = Lelem(i);
 *		use x
 *	}
 * which is like:
 *      'for all elements x of s do'
 *		use x
 */


Lindex Lfirst(lset s)
{
	return ((Lindex) s);
	/* Note that an index for long sets is just
	 * a pointer to an elemholder.
	 */
}


/*ARGSUSED1*/
Lindex Lnext(Lindex i, lset s)
{
	assert(i != (Lindex) 0);
	return (i->e_next);
}


Lelem_t Lelem(Lindex i)
{
	return (i->e_elem);
}



void Ljoin(lset s1, lset *s2_p)
{
	/* Join two sets, assign the result to the second set
	 * and delete the first set (i.e. the value of the
	 * first set becomes undefined).
	 */

	 register elem_p *epp, ep;
	 lset s2;

	 /* First all elements of s1 that are also an element of s2
	  * are removed from the s1 list. The two resulting lists
	  * (for s1 and s2) are linked (s1 first).
	  * Note the usage of epp, which points to a pointer that
	  * points to the next elemholder record of the list.
	  */

	s2 = *s2_p;
	epp = &s1;
	while ((ep = *epp) != (elem_p) 0) {
		if (Lis_elem(ep->e_elem,s2)) {
			/* remove an element */
			*epp = ep->e_next;
			oldelem(ep);
		} else {
			epp = &ep->e_next;
		}
	}
	*epp = s2; /* last record of s1 (or s1 itself) now points
		    * to first record of s2.
		    */
	*s2_p = s1;
}


void Ldeleteset(lset s)
{
	register elem_p ep, next;

	for (ep = s; ep != (elem_p) 0; ep = next) {
		next = ep->e_next;
		oldelem(ep);
	}
}


bool Lis_subset(lset s1, lset s2)
{
	/* See if s1 is a subset of s2 */

	register Lindex i;

	for (i = Lfirst(s1); i != (Lindex) 0; i = Lnext(i,s1)) {
		if (!Lis_elem(Lelem(i),s2)) return FALSE;
	}
	return TRUE;
}


short Lnrelems(lset s)
{
	/* Compute the number of elements of a set */

	register elem_p ep;
	register short  cnt;

	cnt = 0;
	for (ep = s; ep != (elem_p) 0; ep = ep->e_next) {
		cnt++;
	}
	return cnt;
}
