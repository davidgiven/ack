/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  S T R E N G T H   R E D U C T I O N
 *
 *  S R _ C A N D . C
 */


#include <em_mnem.h>
#include <em_pseu.h>
#include "../share/types.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/map.h"
#include "../share/utils.h"
#include "sr.h"
#include "sr_aux.h"
#include "sr_cand.h"


/* A candidate induction variable of a loop (hereafter called candidate) is a
 * local variable (of the current procedure) that is assigned a value
 * precisely once within the loop. Furthermore, this assignment must
 * take place in a firm block of the loop.
 * We determine those locals that are assigned precisely once, within
 * a firm block;
 *
 * We represent a local variable via an instruction that references it,
 * e.g. LOL -6 represents the local variable at offset -6 with size=wordsize.
 * We keep track of two sets:
 *  cand     - the set of all candidate variables
 *  dismiss  - a set of variables that may not be made a candidate
 *		(because they are assigned more than once, or because
 *		 they are assigned outside a firm block).
 * Only local variables for which a register message is given are considered.
 */


STATIC lset cand,		/* set of candidates */
	    dism;		/* set of dismissed variables */


#define ALL_LINES(lnp,list)	lnp = list; lnp != (line_p) 0; lnp = lnp->l_next



STATIC un_cand(lnp)
	line_p lnp;
{
	/* remove the variable stored into by lnp from the list of
	 * candidates (if it was there anyway).
	 */

	Lindex i, next;

	for (i = Lfirst(cand); i != (Lindex) 0; i = next) {
		next = Lnext(i,cand);
		if (same_local(lnp,Lelem(i))) {
			OUTTRACE("remove candidate",0);
			Lremove(Lelem(i), &cand);
		}
	}
}


STATIC bool is_cand(lnp)
	line_p lnp;
{
	/* see if the variable stored into by lnp is a candate */

	Lindex i;

	for (i = Lfirst(cand); i != (Lindex) 0; i = Lnext(i,cand)) {
		if (same_local(lnp,Lelem(i))) {
			return TRUE;
		}
	}
	return FALSE;
}


STATIC make_cand(lnp)
	line_p lnp;
{
	/* make the variable stored into by lnp a candidate */


	OUTTRACE("add a new candidate",0);
	Ladd(lnp,&cand);
}



STATIC do_dismiss(lnp)
	line_p lnp;
{
	Ladd(lnp,&dism);
}


STATIC dismiss(lnp)
	line_p lnp;
{
	/* The variable referenced by lnp is turned definitely into
	 * a non-candidate.
	 */

	un_cand(lnp);	/* remove it from the candidate set,
			 * if it was there in the first place.
			 */
	do_dismiss(lnp); /* add it to the set of dismissed variables */
}


STATIC bool not_dismissed(lnp)
	line_p lnp;
{
	Lindex i;

	for (i = Lfirst(dism); i != (Lindex) 0; i = Lnext(i,dism)) {
		if (same_local(Lelem(i),lnp)) {
			return FALSE; /* variable was dismissed */
		}
	}
	return TRUE;
}


STATIC void try_cand(lnp,b)
	line_p lnp;
	bblock_p b;
{
	/* If the variable stored into by lnp was not already a candidate
	 * and was not dismissed, then it is made a candidate
	 * (unless the assignment takes places in a block that is not firm).
	 */

	if (!is_regvar(off_set(lnp))) return;
	if (is_cand(lnp) || !IS_FIRM(b)) {
		dismiss(lnp);
	} else {
		if (not_dismissed(lnp)) {
			make_cand(lnp);
		}
	}
}


candidates(lp,cand_out,vars_out)
	loop_p lp;
	lset   *cand_out, *vars_out;
{
	/* Find the candidate induction variables.
	 */

	bblock_p b;
	line_p lnp;
	Lindex i;

	OUTTRACE("find candidates of loop %d",lp->lp_id);
	cand = Lempty_set();
	dism = Lempty_set();

	for (i = Lfirst(lp->LP_BLOCKS); i != (Lindex) 0;
					  i = Lnext(i,lp->LP_BLOCKS)) {
		b = (bblock_p) Lelem(i);
		for ( ALL_LINES(lnp, b->b_start)) {
			OUTTRACE("inspect instruction %d",INSTR(lnp));
			switch(INSTR(lnp)) {
				case op_stl:
				case op_inl:
				case op_del:
					OUTTRACE("it's a store local",0);
					try_cand(lnp,b);
					break;
				case op_zrl:
					OUTTRACE("it's a destroy local",0);
					if (is_regvar(off_set(lnp))) {
						dismiss(lnp);
					}
					break;
			}
		}
	}
	*cand_out = cand;
	*vars_out = dism;
}
