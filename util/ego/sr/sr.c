/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* S T R E N G T H   R E D U C T I O N  */


#include <stdlib.h>
#include <stdio.h>
#include "../share/types.h"
#include "sr.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/files.h"
#include "../share/get.h"
#include "../share/put.h"
#include "../share/lset.h"
#include "../share/map.h"
#include "../share/alloc.h"
#include "../share/go.h"
#include "../share/aux.h"
#include "sr_aux.h"
#include "sr_iv.h"

/* Strength reduction tries to change expensive operators occurring
 * in a loop into cheaper operators. The expensive operators considered
 * are multiplication, left-shift and array referencing.
 * The transformations can be expressed in C as:
 *
 * [1]:		for (i = e1; i <= e2; i++)
 *			print(118*i);
 *   becomes:
 *		for (i = e1, t = 118*e1; i <= e2; i++, t += 118)
 *			print(t);
 *
 * [2]:		for (i = e1; i <= e2; i++)
 *			print(a[i]);
 *   becomes:
 *		for (i = e1, p = &a[i]; i <= e2; i++, p++)
 *			print(*p);
 * The latter optimization is suppressed if array bound checking
 * is required.
 */

/* Machine and/or language dependent parameters: */

int ovfl_harmful;
int arrbound_harmful;
int sli_threshold;

int Ssr;  /* #optimizations found */

sr_machinit(f)
	FILE *f;
{
	/* Read target machine dependent information */
	char s[100];


	for (;;) {
		while(getc(f) != '\n');
		fscanf(f,"%s",s);
		if (strcmp(s,"%%SR") == 0)break;
	}
	fscanf(f,"%d",&ovfl_harmful);
	fscanf(f,"%d",&arrbound_harmful);
	fscanf(f,"%d",&sli_threshold);
}

STATIC del_ivs(ivs)
	lset ivs;
{
	/* Delete the set of iv structs */

	Lindex i;

	for (i = Lfirst(ivs); i != (Lindex) 0; i = Lnext(i,ivs)) {
		oldiv(Lelem(i));
	}
	Ldeleteset(ivs);
}


STATIC do_loop(loop)
	loop_p loop;
{
	lset ivs, vars;

	OUTTRACE("going to process loop %d",loop->lp_id);
	induc_vars(loop,&ivs, &vars);
	/* Build a set of iv_structs, one for every induction
	 * variable of the loop, i.e. a variable i that
	 * is changed only by  i := i + c, where c is a loop constant.
	 * Also detects variables that are changed (including induction
	 * variables!).
	 */
	OUTTRACE("loop has %d induction variables",Lnrelems(ivs));
	if (Lnrelems(ivs) > 0) {
		strength_reduction(loop,ivs,vars);
		/* Perform strength reduction. Reduce:
		 *    iv * c    to addition
		 *    a[iv]     to indirection (*p)
		 *     (unless array bound checking is required)
		 */
	}
	del_ivs(ivs);
	Ldeleteset(vars);
}



STATIC loopblocks(p)
	proc_p p;
{
	/* Compute the LP_BLOCKS sets for all loops of p */

	register bblock_p b;
	register Lindex i;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		for (i = Lfirst(b->b_loops); i != (Lindex) 0;
		   i = Lnext(i,b->b_loops)) {
			Ladd(b,&(((loop_p) Lelem(i))->LP_BLOCKS));
		}
	}
}



STATIC opt_proc(p)
	proc_p p;
{
	/* Optimize all loops of one procedure. We first do all
	 * outer loops at the lowest nesting level and proceed
	 * in the inwards direction.
	 */

	Lindex i;
	loop_p lp,outermost;
	int min_level;

	for (;;) {
		min_level = 1000;
		outermost = 0;
		for (i = Lfirst(p->p_loops); i != (Lindex) 0;
		     i = Lnext(i,p->p_loops)) {
			lp = (loop_p) Lelem(i);
			if (!lp->LP_DONE && lp->lp_level < min_level) {
				min_level = lp->lp_level;
				outermost = lp;
			}
		}
		if (! outermost) break;
		do_loop(outermost);
		outermost->LP_DONE = TRUE;
		OUTTRACE("loop %d processed",outermost->lp_id);
	}
}



STATIC bblock_p header(lp)
	loop_p lp;
{
	/* Try to determine the 'header' block of loop lp.
	 * If 'e' is the entry block of loop L, then block 'b' is
	 * called the header block of L, iff:
	 *	SUCC(b) = {e} & b dominates e.
	 * If lp has no header block, 0 is returned.
	 */

	bblock_p x = lp->lp_entry->b_idom;

	if (x != (bblock_p) 0 && Lnrelems(x->b_succ) == 1 &&
	    (bblock_p) Lelem(Lfirst(x->b_succ)) == lp->lp_entry) {
		return x;
	}
	return (bblock_p) 0;
}



STATIC sr_extproc(p)
	proc_p p;
{
	/* Allocate the extended data structures for procedure p */

	register loop_p lp;
	register Lindex pi;

	for (pi = Lfirst(p->p_loops); pi != (Lindex) 0;
	   pi = Lnext(pi,p->p_loops)) {
		lp = (loop_p) Lelem(pi);
		lp->lp_extend = newsrlpx();
		lp->LP_HEADER = header(lp);
		if (lp->LP_HEADER) {
			lp->LP_INSTR = last_instr(lp->LP_HEADER);
		}
	}
}


STATIC sr_cleanproc(p)
	proc_p p;
{
	/* Remove the extended data structures for procedure p */

	register loop_p lp;
	register Lindex pi;


	for (pi = Lfirst(p->p_loops); pi != (Lindex) 0;
	   pi = Lnext(pi,p->p_loops)) {
		lp = (loop_p) Lelem(pi);
		oldsrlpx(lp->lp_extend);
	}
}


void
sr_optimize(p)
	proc_p p;
{
	if (IS_ENTERED_WITH_GTO(p)) return;
	sr_extproc(p);
	loopblocks(p);
	opt_proc(p);
	sr_cleanproc(p);
}



main(argc,argv)
	int argc;
	char *argv[];
{
	go(argc,argv,no_action,sr_optimize,sr_machinit,no_action);
	report("strength reductions",Ssr);
	exit(0);
}
