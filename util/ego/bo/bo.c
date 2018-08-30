/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* B R A N C H   O P T I M I Z A T I O N
 *
 * B O . C
 */


#include <stdlib.h>
#include <stdio.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include <em_spec.h>
#include <em_flag.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/files.h"
#include "../share/get.h"
#include "../share/put.h"
#include "../share/lset.h"
#include "../share/map.h"
#include "../share/alloc.h"
#include "../share/aux.h"
#include "../share/def.h"
#include "../share/go.h"

extern char em_flag[];

#define LP_BLOCKS	lp_extend->lpx_ra.lpx_blocks

#define newbolpx()	(lpext_p)	newstruct(lpext_ra)
#define oldbolpx(x)	oldstruct(lpext_ra,x)

STATIC int Sbo;  /* #optimizations found */

#define DLINK(l1,l2)	l1->l_next=l2; l2->l_prev=l1

/* This module performs some very simple branch optimizations.
 *
 * I) Look for pairs of basic blocks (B1,B2), such that
 *        SUCC(b1) = {B2} and
 *        PRED(B2) = {B1}.
 *    In this case B1 and B2 can be combined into one block.
 *    This optimization is mainly succesful:
 *     1) for switch statements in C, as the C compiler generates a branch
 *        over the entire switch.
 *     2) for return statements, if the only way to return from a procedure
 *        is via a return statement somewhere in the middle of the procedure.
 * II) Optimize while statements. Transformations like:
 *         1:				jmp 2
 *		tst cond		1:
 *		beq 2f			    S
 *		S			2:  
 *		jmp 1			    tst cond
 *	    2: 				    bneq 1
 *	are done by this optimization.
 */



STATIC line_p last_code(lines,skip_pseu)
	line_p lines;
	bool skip_pseu;
{
	/* Determine the last line of a list */

	register line_p l;

	for (l = lines; l->l_next != (line_p) 0; l = l->l_next);
	if (skip_pseu) {
		while (l && (INSTR(l) < sp_fmnem || INSTR(l) > sp_lmnem)) l = PREV(l);
	}
	return l;
}

STATIC short cc_tab[12] = 
	{op_blt,op_zlt,op_ble,op_zle,op_beq,op_zeq,
	 op_zne,op_bne,op_zgt,op_bgt,op_zge,op_bge};


STATIC short rev_cond(cond)
	 short cond;
{
	 register i;

	 for (i = 0; i < 12; i++) {
		if (cond == cc_tab[i]) return cc_tab[11-i];
	}
	return op_nop;
}

STATIC bool is_bcc(l)
	line_p l;
{
	return rev_cond(INSTR(l)) != op_nop;
}


STATIC bo_optloop(p,b,x,bra,bcc)
	proc_p p;
	bblock_p b,x;
	line_p bra,bcc;
{
	bblock_p prevb,n;
	line_p l;

	if (b->b_start == bra) {
		b->b_start = (line_p) 0;
	} else {
		PREV(bra)->l_next = (line_p) 0;
	}
	PREV(bra) = (line_p) 0;
	bcc->l_instr = rev_cond(INSTR(bcc));
	n = x->b_next;
	l = n->b_start;
	if (l == (line_p) 0 || INSTR(l) != op_lab) {
		l = newline(OPINSTRLAB);
		l->l_instr = op_lab;
		INSTRLAB(l) = freshlabel();
		if (n->b_start != (line_p) 0) {
			DLINK(l,n->b_start);
		}
		n->b_start = l;
	}
	INSTRLAB(bcc) = INSTRLAB(l);
	for (prevb = p->p_start; prevb != (bblock_p) 0 && prevb->b_next != x;
		prevb = prevb->b_next);
	if (prevb == (bblock_p) 0) {
		p->p_start = x->b_next;
	} else {
		prevb->b_next = x->b_next;
		l = last_instr(prevb);
		if (l == (line_p) 0) {
			prevb->b_start = bra;
		} else {
		      	if ((em_flag[INSTR(l)-sp_fmnem]&EM_FLO) == FLO_T) {
				oldline(bra);
			} else {
				appnd_line(bra,l);
			}
		}
	}
	x->b_next = b->b_next;
	b->b_next = x;
}

			

STATIC void bo_tryloop(p,loop)
	proc_p p;
	lset loop;
{
	Lindex i,j;
	bblock_p b,x;
	line_p bra,bcc;

	for (i = Lfirst(loop); i != (Lindex) 0; i = Lnext(i,loop)) {
		b = (bblock_p) Lelem(i);
		if (b->b_next != (bblock_p) 0 && !Lis_elem(b->b_next,loop)) {
			j = Lfirst(b->b_succ);
			if (j != (Lindex) 0 &&
			    (bra = last_instr(b)) != (line_p) 0 &&
			    INSTR(bra) == op_bra) {
				x = (bblock_p) Lelem(j); /* single successor */
				if (Lis_elem(b->b_next,x->b_succ) &&
				    is_bcc((bcc = last_instr(x)))) {
OUTVERBOSE("branch optimization proc %d block %d\n", curproc->p_id,x->b_id);
					Sbo++;
					bo_optloop(p,b,x,bra,bcc);
					return;
				}
			}
		}
	}
}



STATIC bo_loops(p)
	proc_p p;
{
	Lindex i;
	loop_p lp;

	for (i = Lfirst(p->p_loops); i != (Lindex) 0; i = Lnext(i,p->p_loops)) {
		lp = (loop_p) (Lelem(i));
		bo_tryloop(p,lp->LP_BLOCKS);
	}
}

STATIC mv_code(b1,b2)
	bblock_p b1,b2;
{
	line_p l,x;

	l = last_code(b2->b_start,TRUE);
	assert(INSTR(l) == op_bra);
	DLINK(l,b1->b_start);
	x = l->l_next;
	rm_line(l,b2);
	if (INSTR(x) == op_lab) {
		rm_line(x,b2);
	}
}

void
bo_switch(b)
	bblock_p b;
{
	bblock_p s,x;
	Lindex i;
	line_p l,bra;

	if (Lnrelems(b->b_succ) == 1) {
		s = (bblock_p) Lelem(Lfirst(b->b_succ));
		if (b->b_start != (line_p) 0 &&
		    s->b_start != (line_p) 0 &&
		    Lnrelems(s->b_pred) == 1 && 
		    (bra = last_code(b->b_start,TRUE)) != (line_p) 0 &&
		    INSTR(bra) == op_bra &&
		    (s->b_next == (bblock_p) 0 ||
		     !Lis_elem(s->b_next,s->b_succ) ||
		     ((bra = last_code(s->b_start, TRUE)) != (line_p) 0 &&
		      (em_flag[INSTR(bra)-sp_fmnem]&EM_FLO) == FLO_T))) {
			l = last_code(s->b_start,FALSE);
			if (INSTR(l) == ps_end) {
				if (PREV(l) == (line_p) 0) return;
				PREV(l)->l_next = (line_p) 0;
				PREV(l) = (line_p) 0;
			} else {
				l = (line_p) 0;
			}
OUTVERBOSE("branch optimization in proc %d, block %d",curproc->p_id,b->b_id);
			Sbo++;
			Ldeleteset(b->b_succ);
			b->b_succ = s->b_succ;
			Ldeleteset(s->b_pred);
			s->b_succ = Lempty_set();
			s->b_pred = Lempty_set();
			for (i = Lfirst(b->b_succ); i != (Lindex) 0;
						i = Lnext(i,b->b_succ)) {
				x = (bblock_p) Lelem(i);
				Lremove(s,&x->b_pred);
				Ladd(b,&x->b_pred);
				if (x->b_idom == s) {
					x->b_idom = b;
				}
			}
			mv_code(s,b);
			s->b_start = l;
		}
	}
}

STATIC bo_extproc(p)
	proc_p p;
{
	/* Allocate the extended data structures for procedure p */

	register loop_p lp;
	register Lindex pi;

	for (pi = Lfirst(p->p_loops); pi != (Lindex) 0;
	   pi = Lnext(pi,p->p_loops)) {
		lp = (loop_p) Lelem(pi);
		lp->lp_extend = newbolpx();
	}
}


STATIC loop_blocks(p)
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

STATIC bo_cleanproc(p)
	proc_p p;
{
	/* Allocate the extended data structures for procedure p */

	register loop_p lp;
	register Lindex pi;
	register bblock_p b;

	for (pi = Lfirst(p->p_loops); pi != (Lindex) 0;
	   pi = Lnext(pi,p->p_loops)) {
		lp = (loop_p) Lelem(pi);
		oldbolpx(lp->lp_extend);
	}
}

void bo_optimize(void *vp)
{
	proc_p p = vp;
	bblock_p b;

	if (IS_ENTERED_WITH_GTO(p)) return;
	bo_extproc(p);
	loop_blocks(p);
	bo_loops(p);
	for (b = p->p_start; b != 0; b = b->b_next) {
		bo_switch(b);
	}
	bo_cleanproc(p);
}



main(argc,argv)
	int argc;
	char *argv[];
{
	go(argc,argv,no_action,bo_optimize,no_action,no_action);
	report("branch optimizations", Sbo);
	exit(0);
}
