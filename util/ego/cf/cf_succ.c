/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  C O N T R O L   F L O W
 *
 *  C F _ S U C C . C
 */


#include <stdio.h>
#include <em_spec.h>
#include <em_pseu.h>
#include <em_flag.h>
#include <em_mnem.h>
#include "../share/types.h"
#include "../share/def.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "cf.h"
#include "../share/map.h"

extern char em_flag[];


STATIC succeeds(succ,pred)
	bblock_p succ, pred;
{
	assert(pred != (bblock_p) 0);
	if (succ != (bblock_p) 0) {
		Ladd(succ, &pred->b_succ);
		Ladd(pred, &succ->b_pred);
	}
}


#define IS_RETURN(i)	(i == op_ret || i == op_rtt)
#define IS_CASE_JUMP(i)	(i == op_csa || i == op_csb)
#define IS_UNCOND_JUMP(i) (i <= sp_lmnem && (em_flag[i-sp_fmnem] & EM_FLO) == FLO_T)
#define IS_COND_JUMP(i)	(i <= sp_lmnem && (em_flag[i-sp_fmnem] & EM_FLO) == FLO_C)
#define TARGET(lnp)	(lbmap[INSTRLAB(lnp)])
#define ATARGET(arg)	(lbmap[arg->a_a.a_instrlab])



STATIC arg_p skip_const(arg)
	arg_p arg;
{
	assert(arg != (arg_p) 0);
	switch(arg->a_type) {
		case ARGOFF:
		case ARGICN:
		case ARGUCN:
			break;
		default:
			error("bad case descriptor");
	}
	return arg->a_next;
}


STATIC arg_p use_label(arg,b)
	arg_p arg;
	bblock_p b;
{
	if (arg->a_type == ARGINSTRLAB) {
		/* arg is a non-null label */
		succeeds(ATARGET(arg),b);
	}
	return arg->a_next;
}



STATIC case_flow(instr,desc,b)
	short    instr;
	line_p   desc;
	bblock_p b;
{
	/* Analyse the case descriptor (given as a ROM pseudo instruction).
	 * Every instruction label appearing in the descriptor
	 * heads a basic block that is a successor of the block
	 * in which the case instruction appears (b).
	 */

	register arg_p arg;

	assert(instr == op_csa || instr == op_csb);
	assert(TYPE(desc) == OPLIST);
	arg = ARG(desc);
	arg = use_label(arg,b);
	/* See if there is a default label. If so, then
	 * its block is a successor of b. Set arg to
	 * next argument.
	 */
	if (instr == op_csa) {
		arg = skip_const(arg); /* skip lower bound */
		arg = skip_const(arg); /* skip lower-upper bound */
		while (arg != (arg_p) 0) {
			/* All following arguments are case labels
			 * or zeroes.
			 */
			arg = use_label(arg,b);
		}
	} else {
		/* csb instruction */
		arg = skip_const(arg);  /* skip #entries */
		while (arg != (arg_p) 0) {
			/* All following arguments are alternatively
			 * an index and an instruction label (possibly 0).
			 */
			arg = skip_const(arg);  /* skip index */
			arg = use_label(arg,b);
		}
	}
}



STATIC line_p case_descr(lnp)
	line_p lnp;
{
	/* lnp is the instruction just before a csa or csb,
	 * so it is the instruction that pushes the address
	 * of a case descriptor on the stack. Find that
	 * descriptor, i.e. a rom pseudo instruction.
	 * Note that this instruction will always be part
	 * of the procedure in which the csa/csb occurs.
	 */

	register line_p l;
	dblock_p d;
	obj_p    obj;
	dblock_id id;

	if (lnp == (line_p) 0 || (INSTR(lnp)) != op_lae) {
		error("cannot find 'lae descr' before csa/csb");
	}
	/* We'll first find the ROM and its dblock_id */
	obj = OBJ(lnp);
	if (obj->o_off != (offset) 0) {
		error("bad 'lae descr' before csa/csb");
		/* We require a descriptor to be an entire rom,
		 * not part of a rom.
		 */
	}
	d = obj->o_dblock;
	assert(d != (dblock_p) 0);
	if (d->d_pseudo != DROM) {
		error("case descriptor must be in rom");
	}
	id = d->d_id;
	/* We'll use the dblock_id to find the defining occurrence
	 * of the rom in the EM text (i.e. a rom pseudo). As all
	 * pseudos appear at the beginning of a procedure, we only
	 * have to look in its first basic block.
	 */
	assert(curproc != (proc_p) 0);
	assert(curproc->p_start != (bblock_p) 0);
	l = curproc->p_start->b_start; /* first instruction of curproc */
	while (l != (line_p) 0) {
		if ((INSTR(l)) == ps_sym &&
		    SHORT(l) == id) {
			/* found! */
			assert((INSTR(l->l_next)) == ps_rom);
			return l->l_next;
		}
		l = l->l_next;
	}
	error("cannot find rom pseudo for case descriptor");
	/* NOTREACHED */
}



STATIC last2_instrs(b,last_out,prev_out)
	bblock_p b;
	line_p   *last_out,*prev_out;
{
	/* Determine the last and one-but-last instruction
	 * of basic block b. An end-pseudo is not regarded
	 * as an instruction. If the block contains only 1
	 * instruction, prev_out is 0.
	 */

	register line_p l1,l2;

	l2 = b->b_start;  /* first instruction of b */
	assert(l2 != (line_p) 0); /* block can not be empty */
	if ((l1 = l2->l_next) == (line_p) 0 || INSTR(l1) == ps_end) {
		*last_out = l2; /* single instruction */
		*prev_out = (line_p) 0;
	} else {
		while(l1->l_next != (line_p) 0 && INSTR(l1->l_next) != ps_end) {
			l2 = l1;
			l1 = l1->l_next;
		}
		*last_out = l1;
		*prev_out = l2;
	}
}



control_flow(head)
	bblock_p head;
{
	/* compute the successor and predecessor relation
	 * for every basic block.
	 */

	register bblock_p b;
	line_p lnp, prev;
	short instr;

	for (b = head; b != (bblock_p) 0; b = b->b_next) {
		/* for every basic block, in textual order, do */
		last2_instrs(b, &lnp, &prev);
		/* find last and one-but-last instruction */
		instr = INSTR(lnp);
		/* The last instruction of the basic block
		 * determines the set of successors of the block.
		 */
		if (IS_CASE_JUMP(instr)) {
			case_flow(instr,case_descr(prev),b);
			/* If lnp is a csa or csb, then the instruction
			 * just before it (i.e. prev) must be the
			 * instruction that pushes the address of the
			 * case descriptor. This descriptor is found
			 * and analysed in order to build the successor
			 * and predecessor sets of b.
			 */
		} else {
		   if (!IS_RETURN(instr)) {
			if (IS_UNCOND_JUMP(instr)) {
				if (instr != op_gto) {
					succeeds(TARGET(lnp),b);
				}
			} else {
				if (IS_COND_JUMP(instr)) {
					succeeds(TARGET(lnp),b);
					succeeds(b->b_next, b);
					/* Textually next block is
					 * a successor of b.
					 */
				} else {
					/* normal instruction */
					succeeds(b->b_next, b);
				}
			}
		   }
		}
	}
}
