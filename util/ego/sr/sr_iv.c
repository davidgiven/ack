/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* S T R E N G T H   R E D U C T I O N
 *
 * S R _ I V . C
 *
 */


#include <em_mnem.h>
#include <em_pseu.h>
#include "../share/types.h"
#include "sr.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/alloc.h"
#include "../share/aux.h"
#include "sr_aux.h"
#include "sr_cand.h"
#include "sr_iv.h"



STATIC lset ivvars;	/* set of induction variables */

STATIC short nature(lnp)
	line_p lnp;
{
	/* Auxiliary routine used by inc_or_dec, is_add and plus_or_min.
	 * Determine if lnp had INCREMENT/DECREMENT-nature (1),
	 * ADD-nature (2), SUBTRACT-nature (3)
	 * or Buddha-nature (0).
	 */

	bool size_ok;

	assert(lnp != (line_p) 0);
	size_ok = (TYPE(lnp) == OPSHORT && SHORT(lnp) == ws);
	switch(INSTR(lnp)) {
		case op_inc:
		case op_dec:
			return 1;
		case op_adi:
		case op_adu:
			return (size_ok? 2:0);
		case op_sbi:
		case op_sbu:
			return (size_ok? 3:0);
	}
	return 0;
}



#define is_add(l)	(nature(l) == 2)
#define plus_or_min(l)	(nature(l) > 1)
#define inc_or_dec(l)	(nature(l) == 1)


STATIC bool is_same(l,lnp)
	line_p l, lnp;
{
	/* lnp is a STL x , where x is a candidate
	 * induction variable. See if l is a LOL x
	 * (with the same x as the store-instruction)
	 */

	assert(INSTR(lnp) == op_stl);
	return l != (line_p) 0 && INSTR(l) == op_lol && 
		off_set(l) == off_set(lnp);
}


STATIC ivar(lnp,step)
	line_p	lnp;
	int	step;
{
	/* Record the fact that we've found a new induction variable.
	 * lnp points to the last instruction of the code that
	 * increments the induction variable, i.e. a STL, DEL or INL.
	 */

	iv_p i;

	i = newiv();
	i->iv_off = (TYPE(lnp) == OPSHORT ? (offset) SHORT(lnp) : OFFSET(lnp));
	i->iv_incr = lnp;	/* last instruction of increment code */
	i->iv_step = step;	/* step value */
	Ladd(i,&ivvars);
}


STATIC int sign(lnp)
	line_p lnp;
{
	switch(INSTR(lnp)) {
		case op_inc:
		case op_inl:
		case op_adi:
		case op_adu:
			return 1;
		case op_dec:
		case op_del:
		case op_sbi:
		case op_sbu:
			return (-1);
		default:
			assert(FALSE);
	}
	/* NOTREACHED */
}


STATIC void try_patterns(lnp)
	line_p lnp;
{
	/* lnp is a STL x; try to recognize
	 * one of the patterns:
	 *  'LOAD const; LOAD x; ADD; STORE x'
	 * or 'LOAD x; LOAD const; ADD or SUBTRACT;
	 *     STORE x'
	 * or 'LOAD x; INCREMENT/DECREMENT; STORE x'
	 */

	line_p l, l2;

	l = PREV(lnp); /* instruction before lnp*/
	if (l == (line_p) 0) return;  /* no match possible */
	l2 = PREV(l);
	if (inc_or_dec(l)) {
		if (is_same(l2,lnp)) {
			/* e.g. LOL iv ; INC ; STL iv */
			ivar(lnp,sign(l));
		}
		return;
	}
	if (is_add(lnp)) {
		if(is_same(l2,lnp) && is_const(PREV(l2))) {
			ivar(lnp,SHORT(PREV(l2)));
			return;
		}
	}
	if (plus_or_min(l)) {
		if (is_const(l2) && is_same(PREV(l2),lnp)) {
			ivar(lnp,sign(l) * SHORT(l2));
		}
	}
}


induc_vars(loop,ivar_out, vars_out)
	loop_p loop;
	lset   *ivar_out, *vars_out;
{
	/* Construct the set of induction variables. We use several
	 * global variables computed by 'candidates'.
	 */

	Lindex i;
	line_p lnp;
	lset cand_iv, vars;

	ivvars = Lempty_set();
	candidates(loop, &cand_iv, &vars);
	/* Find the set of all variables that are assigned precisely
	 * once within the loop, within a firm block.
	 * Also find all remaining local variables that are changed
	 * within the loop.
	 */
	if (Lnrelems(cand_iv) > 0) {
		for (i = Lfirst(cand_iv); i != (Lindex) 0; i = Lnext(i,cand_iv)) {
			lnp = (line_p) Lelem(i);
			if (INSTR(lnp) == op_inl || INSTR(lnp) == op_del) {
				ivar(lnp,sign(lnp));
			} else {
				try_patterns(lnp);
			}
		}
	}
	Ljoin(cand_iv, &vars);
	*ivar_out = ivvars;
	*vars_out = vars;
}
