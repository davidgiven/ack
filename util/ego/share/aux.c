/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  S H A R E D   F I L E
 *
 *  A U X I L I A R Y   R O U T I N E S
 *
 */

#include <em_mes.h>
#include <em_pseu.h>
#include "types.h"
#include "debug.h"
#include "global.h"
#include "alloc.h"
#include "aux.h"
#include "map.h"
#include "lset.h"

offset off_set(lnp)
	line_p lnp;
{
	switch(lnp->l_optype) {
		case OPSHORT:
			return (offset) SHORT(lnp);
		case OPOFFSET:
			return OFFSET(lnp);
		default:
			assert(FALSE);
	}
	/* NOTREACHED */
}




offset aoff(ap,n)
	register arg_p ap;
{
	while (n>0) {
		if (ap != (arg_p) 0)
			ap = ap->a_next;
		n--;
	}
	if (ap == (arg_p) 0)
		error("too few parameters");
	if (ap->a_type != ARGOFF)
		error("offset expected");
	return(ap->a_a.a_offset);
}


offset tmplocal(p,size)
	proc_p p;
	offset   size;
{
	/* Allocate a new local variable in the stack frame of p */

	p->p_localbytes +=  size;
	return -(p->p_localbytes);
}




line_p int_line(off)
	offset off;
{
	/* Allocate a line struct of type OPSHORT or OPOFFSET,
	 * whichever one fits best.
	 */

	line_p lnp;

	if ((short) off == off) {
		/* fits in a short */
		lnp = newline(OPSHORT);
		SHORT(lnp) = (short) off;
	} else {
		lnp = newline(OPOFFSET);
		OFFSET(lnp) = off;
	}
	return lnp;
}



line_p reg_mes(tmp,size,typ,score)
	offset tmp;
	short  size;
	int    typ,score;
{
	/* Generate a register message */

	line_p l;
	arg_p a;

#define NEXTARG(a,val) a->a_next = newarg(ARGOFF); a = a->a_next; \
			a->a_a.a_offset = val
	l = newline(OPLIST);
	l->l_instr = ps_mes;
	a = ARG(l) = newarg(ARGOFF);
	a->a_a.a_offset = ms_reg;
	NEXTARG(a,tmp);
	NEXTARG(a,size);
	NEXTARG(a,typ);
	NEXTARG(a,score);
	return l;
}


bool dom(b1,b2)
	bblock_p b1,b2;
{
	/* See if b1 dominates b2. Note that a block always
	 * dominates itself.
	 */

	register bblock_p b;

	for (b = b2; b != (bblock_p) 0; b = b->b_idom) {
		/* See if b1 is a (not necessarily proper) ancestor
		 * of b2 in the immediate dominator tree.
		 */
		if (b == b1) return TRUE;
	}
	return FALSE;
}


bblock_p common_dom(a,b)
	bblock_p a,b;
{
	/* find a basic block that dominates a as well as b;
	 * note that a basic block also dominates itself.
	 */

	assert (a != (bblock_p) 0);
	assert (b != (bblock_p) 0);
	if (dom(a,b)) {
		return a;
	} else {
		if (dom(b,a)) {
			return b;
		} else {
			return common_dom(a->b_idom,b->b_idom);
		}
	}
}

#define R	time_space_ratio

short add_timespace(time,space)
	short time,space;
{
	/* Add together a time and space, using the time_space_ratio
	 * parameter that may be set by the user, indicating the need
	 * to optimize for time, space or something in between.
	 */

	return (R * time + (100 - R) * space) / 100;
}



rm_line(l,b)
	line_p l;
	bblock_p b;
{
	if (b->b_start == l) {
		b->b_start = l->l_next;
	} else {
		PREV(l)->l_next = l->l_next;
	}
	if (l->l_next != (line_p) 0) {
		PREV(l->l_next) = PREV(l);
	}
	oldline(l);
}




appnd_line(l1,l2)
	line_p l1,l2;
{
	/* Put l1 after l2 */

	PREV(l1) = l2;
	l1->l_next = l2->l_next;
	l2->l_next = l1;
	if (l1->l_next != (line_p) 0) {
		PREV(l1->l_next) = l1;
	}
}



line_p last_instr(b)
	bblock_p b;
{
	/* Determine the last line of a list */

	register line_p l = b->b_start;

	if (l == (line_p) 0) return (line_p) 0;
	while (l->l_next != (line_p) 0) l = l->l_next;
	return l;
}




line_p find_mesreg(off)
	offset off;
{
	/* Find the register message for the local with the given offset */

	Lindex li;
	line_p l;

	for (li = Lfirst(mesregs); li != (Lindex) 0; li = Lnext(li,mesregs)) {
		l = (line_p) Lelem(li);
		if (aoff(ARG(l),1) == off) return l;
	}
	return (line_p) 0;
}


bool is_regvar(off)
	offset off;
{
	return find_mesreg(off) != (line_p) 0;
}



offset regv_arg(off,n)
	offset off;
	int n;
{
	/* fetch the n'th argument of the register message of the
	 * local variable at offset off;
	 */

	line_p x = find_mesreg(off);
	assert (x != (line_p) 0);
	return aoff(ARG(x),n);
}
