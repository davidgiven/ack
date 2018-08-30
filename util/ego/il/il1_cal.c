/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 1 _ C A L . C
 */

#include <stdio.h>
#include <em_spec.h>
#include <em_mnem.h>
#include "../share/types.h"
#include "il.h"
#include "il1_cal.h"
#include "../share/debug.h"
#include "../share/alloc.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "il1_aux.h"
#include "../share/parser.h"

STATIC actual_p acts, *app;

#define INIT_ACTS()	{acts = (actual_p) 0; app = &acts;}
#define APPEND_ACTUAL(a) {*app = a; app = &a->ac_next;}

STATIC void make_actual(line_p l1, line_p l2, offset size)
{
	/* Allocate a struct for a new actual parameter
	 * expression, the code of which extends from
	 * l1 to l2.
	 */

	actual_p a;

	a = newactual();
	a->ac_exp = copy_code(l1,l2);
	a->ac_size = size;
	APPEND_ACTUAL(a); /* append it to actual-list */
}



STATIC bool chck_asp(p,l)
	proc_p p;
	line_p l;
{
	/* We require a call to a procedure p that has n formal
	 * parameters to be followed by an 'asp n' instruction
	 * (i.e. the caller should remove the actual parameters).
	 */

	return (p->p_nrformals == 0 || (l != (line_p) 0 &&INSTR(l) == op_asp &&
		   TYPE(l) == OPSHORT && SHORT(l) == p->p_nrformals));
}



STATIC void inc_count(caller,callee)
	proc_p caller, callee;
{
	/* Update the call-count information.
	 * Record the fact that there is one more call
	 * to 'callee', appearing in 'caller'.
	 */

	calcnt_p cc;

	if (!SUITABLE(caller)) return;
	/* if the calling routine is never expanded in line
	 * we do not need call-count information.
	 */
	for (cc = cchead; cc != (calcnt_p) 0; cc = cc->cc_next) {
		if (cc->cc_proc == callee) {
			cc->cc_count++;
			/* #calls to callee from caller */
			return;
		}
	}
	/* This is the first call from caller to callee.
	 * Allocate a new calcnt struct.
	 */
	cc = newcalcnt();
	cc->cc_proc = callee;
	cc->cc_count = 1;
	cc->cc_next = cchead; /* insert it at front of list */
	cchead = cc;
}



anal_cal(p,call,b,cf)
	proc_p p;
	line_p call;
	bblock_p b;
	FILE   *cf;
{
	/* Analyze a call instruction. If the called
	 * routine may be expanded in line, try to
	 * recognize the actual parameter expressions of
	 * the call and extend the call list.
	 */

	call_p c;
	line_p lnp;
	proc_p callee;

#ifdef VERBOSE
	Scals++;
#endif
	calnr++;
	callee = PROC(call);
	if (SUITABLE(callee)) {
		/* The called procedure may be expanded */
		callee->P_NRCALLED++;   /* #calls to callee from anywhere */
		INIT_ACTS();
		if (parse(PREV(call),callee->p_nrformals,&lnp,0,make_actual) &&
		      chck_asp(callee,call->l_next)) {
			/* succeeded in recognizing the actuals */
			c = newcall();
			c->cl_caller = p;
			c->cl_id = calnr;
			c->cl_proc = callee;
			c->cl_looplevel = (byte) looplevel(b);
			if (c->cl_looplevel > 0 && IS_FIRM(b)) {
				c->cl_flags |= CLF_FIRM;
			}
			c->cl_actuals = acts;
			inc_count(p,callee);
			/* update call-count info */
			putcall(c,cf,(short) 0);  /* write the call to the calfile */
		} else {
#ifdef VERBOSE
			Sparsefails++;
#endif
			rem_actuals(acts);
		}
	}
}
