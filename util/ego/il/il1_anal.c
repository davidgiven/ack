/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 1 _ A N A L . C
 */

#include <stdio.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include "../share/types.h"
#include "il.h"
#include "../share/debug.h"
#include "../share/alloc.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/aux.h"
#include "il1_aux.h"
#include "il1_formal.h"
#include "il1_cal.h"
#include "il1_anal.h"
#include "il_aux.h"
#include "../share/put.h"

#define ENVIRON(p)		(p->p_flags1 & (byte) PF_ENVIRON)
#define RETURN_BLOCK(b)		(Lnrelems(b->b_succ) == 0)
#define LAST_BLOCK(b)		(b->b_next == (bblock_p) 0)

/* Daisy chain recursion not yet accounted for: */
#define RECURSIVE(p)		(Cis_elem(p->p_id,p->p_calling))
/*
#define CALLS_UNKNOWN(p)	(p->p_flags1 & (byte) PF_CALUNKNOWN)
*/
#define CALLS_UNKNOWN(p)	(FALSE)



apriori(proctab)
	proc_p proctab;
{
	/* For every procedure, see if we can determine
	 * from the information provided by the previous
	 * phases of the optimizer that it cannot or should not
	 * be expanded in line. This will reduce the length
	 * of the call list.
	 */

	register proc_p p;

	for (p = proctab; p != (proc_p) 0; p = p->p_next) {
		if (!BODY_KNOWN(p)  ||
		     ENVIRON(p) || RECURSIVE(p) ||
		     PARAMS_UNKNOWN(p) || MANY_LOCALS(p) ||
		     IS_ENTERED_WITH_GTO(p)) {
			 UNSUITABLE(p);
#ifdef VERBOSE
			if (BODY_KNOWN(p)) {
				if (ENVIRON(p)) Senv++;
				if (RECURSIVE(p)) Srecursive++;
				if (MANY_LOCALS(p)) Slocals++;
			}
#endif
		}
	}
}


STATIC check_labels(p,arglist)
	proc_p p;
	arg_p arglist;
{
	/* Check if any of the arguments contains an instruction
	 * label; if so, make p unsuitable.
	 */

	arg_p arg;

	for (arg = arglist; arg != (arg_p) 0; arg = arg->a_next) {
		if (arg->a_type == ARGINSTRLAB) {
			UNSUITABLE(p);
#ifdef VERBOSE
			Sinstrlab++;
#endif
			break;
		}
	}
}



STATIC anal_instr(p,b,cf)
	proc_p   p;
	bblock_p b;
	FILE     *cf;
{
	/* Analyze the instructions of block b
	 * within procedure p.
	 * See which parameters are used, changed
	 * or have their address taken. Recognize
	 * the actual parameter expressions of
	 * the CAL instructions.
	 */

	register line_p l;

	for (l = b->b_start; l != (line_p) 0; l = l->l_next) {
	   switch(INSTR(l)) {
		case op_cal:
			anal_cal(p,l,b,cf);
			break;
		case op_stl:
		case op_inl:
		case op_del:
		case op_zrl:
			formal(p,b,off_set(l),SINGLE,CHANGE);
			/* see if the local is a parameter.
			 * If so, it is a one-word parameter
			 * that is stored into.
			 */
			break;
		case op_sdl:
			formal(p,b,off_set(l),DOUBLE,CHANGE);
			break;
		case op_lol:
			formal(p,b,off_set(l),SINGLE,USE);
			break;
		case op_ldl:
			formal(p,b,off_set(l),DOUBLE,USE);
			break;
		case op_sil:
		case op_lil:
			formal(p,b,off_set(l),POINTER,USE);
			break;
		case op_lal:
			formal(p,b,off_set(l),UNKNOWN,ADDRESS);
			break;
		case ps_rom:
		case ps_con:
		case ps_bss:
		case ps_hol:
			check_labels(p,ARG(l));
			break;
	   }
	}
}



anal_proc(p,cf,ccf)
	proc_p p;
	FILE   *cf,*ccf;
{
	/* Analyze a procedure; use information
	 * stored in its basic blocks or in
	 * its instructions.
	 */

	register bblock_p b;
	bool     fallthrough = TRUE;

	cchead = (calcnt_p) 0;
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		if (RETURN_BLOCK(b) && !LAST_BLOCK(b)) {
			fallthrough = FALSE;
			/* p contains a RET instruction somewhere
			 * in the middle of its code.
			 */
		}
		anal_instr(p,b,cf); /* analyze instructions */
	}
	if (fallthrough) {
		p->p_flags2 |= PF_FALLTHROUGH;
	}
	rem_indir_acc(p);
	/* don't expand formal that may be accessed indirectly */
	p->P_CCADDR = putcc(cchead,ccf);
	/* write calcnt info and remember disk address */
	remcc(cchead);
}
