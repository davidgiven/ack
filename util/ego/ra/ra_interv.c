/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ I N T E R V A L . C
 */


#include <em_reg.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/alloc.h"
#include "../share/lset.h"
#include "ra.h"
#include "ra_interv.h"

interv_p cons_interval(t_start,t_stop)
	short t_start,t_stop;
{
	interv_p x;

	x = newinterval();
	x->i_start = t_start;
	x->i_stop = t_stop;
	return x;
}



add_interval(t1,t2,list)
	short t1,t2;
	interv_p *list;
{
	/* Add interval (t1,t2) to the list of intervals (which is
	 * an in-out parameter!). The list is sorted in 'chronological'
	 * order. We attempt to keep the list as small as possible, by
	 * putting adjacent intervals in one interval.
	 */

	register interv_p x1, x2, *q;
	int adjacent = 0;
	interv_p x;

	q = list;
	x1 = (interv_p) 0;
	for (x2 = *list; x2 != (interv_p) 0; x2 = x2->i_next) {
		if (t2 < x2->i_start) break;
		x1 = x2;
		q = &x2->i_next;
	}
	/* Now interval (t1,t2) should be inserted somewhere in between
	 * x1 and x2.
	 */
	if (x1 != (interv_p) 0 && t1 == x1->i_stop + 1) {
		/* join x1 and (t1,t2) */
		x1->i_stop = t2;
		adjacent++;
	}
	if (x2 != (interv_p) 0 && t2 + 1 == x2->i_start) {
		/* join (t1,t2) and x2 */
		x2->i_start = t1;
		adjacent++;
	}
	if (adjacent == 0) {
		/* no adjacents, allocate a new intervalfor (t1,t2) */
		x = cons_interval(t1,t2);
		x->i_next = x2;
		*q = x;
	} else {
		if (adjacent == 2) {
			/* x1, (t1,t2) and x2 can be put in one interval */
			x1->i_stop = x2->i_stop;
			x1->i_next = x2->i_next;
			oldinterval(x2);
		}
	}
}



interv_p loop_lifetime(lp)
	loop_p lp;
{
	/* Determine the timespan of the loop, expressed as a list
	 * of intervals.
	 */

	interv_p lt = 0;
	register bblock_p b;
	register Lindex bi;

	for (bi = Lfirst(lp->LP_BLOCKS); bi != (Lindex) 0;
					 bi = Lnext(bi,lp->LP_BLOCKS)) {
		b = (bblock_p) Lelem(bi);
		add_interval(b->B_BEGIN,b->B_END,&lt);
	}
	return lt;
}


interv_p proc_lifetime(p)
	proc_p p;
{
	/* Determine the lifetime of an entire procedure */

	register bblock_p b;

	for (b = p->p_start; b->b_next != (bblock_p) 0; b = b->b_next) ;
	return cons_interval(0,b->B_END);
}



STATIC set_min_max(iv1,iv2)
	interv_p *iv1,*iv2;
{
	/* Auxiliary routine of intersect */

	interv_p i1 = *iv1, i2 = *iv2;

	if (i1->i_start < i2->i_start) {
		*iv1 = i1;
		*iv2 = i2;
	} else {
		*iv1 = i2;
		*iv2 = i1;
	}
}



interv_p intersect(list1,list2)
	interv_p list1,list2;
{
	/* Intersect two lifetimes, each denoted by a list of intervals.
	 * We maintain two pointers, pmin and pmax, pointing to the
	 * next interval of each list. At any time, pmin points to the
	 * interval of which i_start is lowest; pmax points to the
	 * other interval (i.e. the next interval of the other list).
	 */

	interv_p lt = 0;
	interv_p pmin,pmax;

#define BUMP(p)	p = p->i_next
#define EMIT(t1,t2)	add_interval(t1,t2,&lt)

	pmin = list1;
	pmax = list2;
	while (pmin != (interv_p) 0 && pmax != (interv_p) 0) {
		set_min_max(&pmin,&pmax);
		if (pmax->i_start > pmin->i_stop) {
			/* e.g. (5,7) and (9,13) */
			BUMP(pmin);
		} else {
			if (pmax->i_stop < pmin->i_stop) {
				/* e.g. (5,12) and (7,10) */
				EMIT(pmax->i_start,pmax->i_stop);
				BUMP(pmax);
			} else {
				/* e.g. (5,8) and (7,12) */
				EMIT(pmax->i_start,pmin->i_stop);
				if (pmax->i_stop == pmin->i_stop) {
					/* e.g. (5,12) and (7,12) */
					BUMP(pmax);
				}
				BUMP(pmin);
			}
		}
	}
	return lt;
}



bool not_disjoint(list1,list2)
	interv_p list1,list2;
{
	/* See if list1 and list2 do overlap somewhere */

	interv_p pmin,pmax;

	pmin = list1;
	pmax = list2;
	while (pmin != (interv_p) 0 && pmax != (interv_p) 0) {
		set_min_max(&pmin,&pmax);
		if (pmax->i_start > pmin->i_stop) {
			/* e.g. (5,7) and (9,13) */
			BUMP(pmin);
		} else {
			return TRUE; /* not disjoint */
		}
	}
	return FALSE; /* disjoint */
}



bool contains(t,timespan)
	short t;
	interv_p timespan;
{
	register interv_p iv;

	for (iv = timespan; iv != (interv_p) 0; iv = iv->i_next) {
		if (t <= iv->i_stop) return (t >= iv->i_start);
	}
	return FALSE;
}



interv_p copy_timespan(list)
	interv_p list;
{
	/* copy the time span */

	interv_p x,y,head,*p;

	head = (interv_p) 0;
	p = &head;

	for (x = list; x != (interv_p) 0; x = x->i_next) {
		y = cons_interval(x->i_start,x->i_stop);
		*p = y;
		p = &y->i_next;
	}
	return head;
}
