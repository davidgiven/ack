/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* S T R E N G T H   R E D U C T I O N
 *
 * S R _ A U X . C
 *
 */


#include <em_mnem.h>
#include <em_pseu.h>
#include "../share/types.h"
#include "sr.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/aux.h"
#include "sr_aux.h"
#include "sr_xform.h"

#define INSIDE_LOOP(b,lp)  Lis_elem(b,lp->LP_BLOCKS)


bool is_loopconst(lnp,vars)
	line_p lnp;
	lset   vars;
{
	Lindex i;

	assert(TYPE(lnp) == OPSHORT || TYPE(lnp) == OPOFFSET);
	if (!is_regvar(off_set(lnp))) return FALSE;
	for (i = Lfirst(vars); i != (Lindex) 0; i = Lnext(i,vars)) {
		if (same_local(Lelem(i),lnp)) {
			return FALSE; /* variable was changed */
		}
	}
	return TRUE;
}


bool is_caddress(lnp,vars)
	line_p lnp;
	lset   vars;  /* variables changed in loop */
{
	/* See if lnp is a single instruction (i.e. without arguments)
	 * that pushes a loop-invariant entity of size pointer-size (ps)
	 * on the stack.
	 */

	if (lnp == (line_p) 0) return FALSE;
	switch(INSTR(lnp)) {
		case op_lae:
		case op_lal:
			return TRUE;
		case op_lol:
			return ps == ws && is_loopconst(lnp,vars);
		case op_ldl:
			return ps == 2*ws && is_loopconst(lnp,vars);
		default:
			return FALSE;
	}
	/* NOTREACHED */
}



STATIC arg_p find_arg(n,list)
	int n;
	arg_p list;
{
	/* Find the n-th element of the list */

	while (--n) {
		if (list == (arg_p) 0) break;
		list = list->a_next;
	}
	return list;
}


int elemsize(lnp)
	line_p lnp;
{
	/* lnp is an instruction that loads the address of an array
	 * descriptor. Find the size of the elements of the array.
	 * If this size cannot be determined (e.g. the descriptor may
	 * not be in a rom) then return UNKNOWN_SIZE.
	 */

	dblock_p d;
	arg_p v;

	assert (lnp != (line_p) 0);
	if (INSTR(lnp) == op_lae) {
		d = OBJ(lnp)->o_dblock; /* datablock */
		if (d->d_pseudo == DROM  &&
		    (v = find_arg(3,d->d_values)) != (arg_p) 0 &&
		    v->a_type == ARGOFF) {
			return (int) v->a_a.a_offset;
		}
	}
	return UNKNOWN_SIZE;
}



concatenate(list1,list2)
	line_p list1,list2;
{
	/* Append list2 to the end of list1. list1 may not be empty. */

	register line_p l;

	assert(list1 != (line_p) 0);
	for (l =list1; l->l_next != (line_p) 0; l = l->l_next);
	l->l_next = list2;
}
