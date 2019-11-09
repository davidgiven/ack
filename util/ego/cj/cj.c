/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* C R O S S   J U M P I N G
 *
 * CJ.H 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <em_mnem.h>
#include <em_spec.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/files.h"
#include "../share/get.h"
#include "../share/put.h"
#include "../share/lset.h"
#include "../share/map.h"
#include "../share/alloc.h"
#include "../share/utils.h"
#include "../share/def.h"
#include "../share/stack_chg.h"
#include "../share/go.h"


/* Cross jumping performs optimzations like:
 * 
 *	 if cond then goto L1;			 if cond then goto L1
 *	 S1;			 ----->		 S1;
 *	 S2;					 goto L3;
 *	 goto L2;				L1:
 *	L1:					 S3;
 *	 S3;					L3:
 *	 S2;					 S2;
 *	L2:
 *
 * CJ looks for two basic blocks b1 and b2 with the following properties:
 *  - there exists a basic block S such that SUCC(b1) = SUCC(b2) = {S}
 *    (so both have only 1 successor)
 *  - the last N (N > 0) instructions of b1 and b2, not counting a possible
 *    BRAnch instruction, are the same.
 * As a result of the first condition, at least of the two blocks must end
 * on an (unconditional) BRAnch instruction. If both end on a BRA, one block
 * is chosen at random. Assume this block is b1. A new label L is put just
 * before the N common instructions of block b2 (so this block is split
 * into two). The BRA of b1 is changed into a BRA L. So dynamically the same
 * instructions are executed in a slightly different order; yet the size of
 * the code has become smaller.
 */


STATIC int Scj;  /* number of optimizations found */

STATIC void showinstr();



#define DLINK(l1,l2)	l1->l_next=l2; l2->l_prev=l1


STATIC bool same_instr(l1,l2)
	line_p l1,l2;
{
	/* See if l1 and l2 are the same instruction */

	if (l1 == 0 || l2 == 0 || TYPE(l1) != TYPE(l2)) return FALSE;
	if (INSTR(l1) != INSTR(l2)) return FALSE;
	switch(TYPE(l1)) {
		case OPSHORT: return SHORT(l1) == SHORT(l2);
		case OPOFFSET: return OFFSET(l1) == OFFSET(l2);
		case OPPROC: return PROC(l1) == PROC(l2);
		case OPOBJECT: return OBJ(l1) == OBJ(l2);
		case OPINSTRLAB: return INSTRLAB(l1) == INSTRLAB(l2);
		case OPNO: return TRUE;
		default: return FALSE;
	}
}



STATIC line_p last_mnem(b)
	bblock_p b;
{
	/* Determine the last line of a list */

	register line_p l;

	for (l = b->b_start; l->l_next != (line_p) 0; l = l->l_next);
	while (l != (line_p) 0 && (INSTR(l) < sp_fmnem || INSTR(l) > sp_lmnem)) {
		l = PREV(l);
	}
	return l;
}


STATIC bool is_desirable(text)
	line_p text;
{
	/* We avoid to generate a BRAnch in the middle of some expression,
	 * as the code generator will write the contents of the fakestack
	 * to the real stack if it encounters a BRA. We do not avoid to
	 * split the parameter-pushing code of a subroutine call into two,
	 * as the parameters are pushed on the real stack anyway.
	 * So e.g. "LOL a ; LOL b; ADI" will not be split, but
	 * "LOL a; LOL b; CAL f" may be split.
	 */

	line_p l;
	bool ok;
	int stack_diff,pop,push;

	stack_diff = 0;
	for (l = text; l != (line_p) 0; l = l->l_next) {
		switch(INSTR(l)) {
			case op_cal:
			case op_asp:
			case op_bra:
				return TRUE;
		}
		line_change(l,&ok,&pop,&push);
		/* printf("instr %d, pop %d, push %d, ok %d\n",INSTR(l),pop,push,ok); */
		if (!ok || (stack_diff -= pop) < 0) {
			return FALSE;
		} else {
			stack_diff += push;
		}
	}
	return TRUE;
}


STATIC void cp_loops(b1,b2)
	bblock_p b1,b2;
{
	/* Copy the loopset of b2 to b1 */

	Lindex i;
	loop_p lp;
	for (i = Lfirst(b2->b_loops); i != (Lindex) 0;
	     i = Lnext(i,b2->b_loops)) {
		lp = (loop_p) Lelem(i);
		Ladd(lp,&b1->b_loops);
	}
}


STATIC void jump_cross(l1,l2,b1,b2)
	line_p l1,l2;
	bblock_p b1,b2;
{
	/* A cross-jump from block b2 to block b1 is found; the code in
	 * block b2 from line l2 up to the BRAnch is removed; block b1 is
	 * split into two; the second part consists of a new label
	 * followed by the code from l1 till the end of the block.
	 */

	line_p l;
	bblock_p b;
	bblock_p s;

	/* First adjust the control flow graph */
	b = freshblock();  /* create a new basic block */
	b->b_succ = b1->b_succ;
	/* SUCC(b1) = {b} */
	b1->b_succ = Lempty_set(); Ladd(b,&b1->b_succ);
	/* SUCC(b2) = {b} */
	Ldeleteset(b2->b_succ); b2->b_succ = Lempty_set(); Ladd(b,&b2->b_succ);
	/* PRED(b) = {b1,b2} */
	b->b_pred = Lempty_set(); Ladd(b1,&b->b_pred); Ladd(b2,&b->b_pred);
	/* PRED(SUCC(b)) := PRED(SUCC(b)) - {b1,b2} + {b} */
	assert(Lnrelems(b->b_succ) == 1); 
	s = (bblock_p) Lelem(Lfirst(b->b_succ));
	Lremove(b1,&s->b_pred); Lremove(b2,&s->b_pred); Ladd(b,&s->b_pred);
	cp_loops(b,b1);
	b->b_idom = common_dom(b1,b2);
	b->b_flags = b1->b_flags;
	b->b_next = b1->b_next;
	b1->b_next = b;

	/* Now adjust the EM text */
	l = PREV(l1);
	while (l && INSTR(l) == op_lab) {
		l1 = l;
		l = PREV(l);
	}
	if (l == (line_p) 0) {
		b1->b_start = (line_p) 0;
	} else {
		l->l_next = (line_p) 0;
	}
	if (INSTR(l1) == op_lab) {
		l = l1;
	}
	else {
		l = newline(OPINSTRLAB);
		l->l_instr = op_lab;
		INSTRLAB(l) = freshlabel();
		DLINK(l,l1);
	}
	b->b_start = l;
	for (l = l2; INSTR(l) != op_bra;) {
		line_p next = l->l_next;

		assert (l != (line_p) 0);
		rm_line(l,b2);
		l = next;
	}
	INSTRLAB(l) = INSTRLAB(b->b_start);
}


STATIC bool try_tail(b1,b2)
	bblock_p b1,b2;
{
	/* See if b1 and b2 end on the same sequence of instructions */

	line_p l1,l2;
	bblock_p b = (bblock_p) 0;
	int cnt = 0;
	/* printf("try block %d and %d\n",b1->b_id,b2->b_id); */

	if (b1->b_start == (line_p) 0 || b2->b_start == (line_p) 0) return FALSE;
	l1 = last_mnem(b1);
	l2 = last_mnem(b2);
	if (l1 == (line_p) 0 || l2 == (line_p) 0) return FALSE;
	/* printf("consider:\n"); showinstr(l1); showinstr(l2); */
	if (INSTR(l1) == op_bra) {
		b = b1;
		l1 = PREV(l1);
	}
	if (INSTR(l2) == op_bra) {
		b = b2;
		l2 = PREV(l2);
	}
	assert(b != (bblock_p) 0);
	while(same_instr(l1,l2)) {
		cnt++;
		l1 = PREV(l1);
		l2 = PREV(l2);
		/* printf("consider:\n"); showinstr(l1); showinstr(l2); */
	}
	if (cnt >= 1) {
		l1 = (l1 == 0 ? b1->b_start : l1->l_next);
		l2 = (l2 == 0 ? b2->b_start : l2->l_next);
		if (is_desirable(l1)) {
			if (b == b1) {
				jump_cross(l2,l1,b2,b1);
				Scj++;
			} else {
				jump_cross(l1,l2,b1,b2);
				Scj++;
			}
			return TRUE;
		}
	}
	return FALSE;
}



STATIC bool try_pred(b)
	bblock_p b;
{
	/* See if there is any pair (b1,b2), both in PRED(b) for
	 * which we can perform cross jumping.
	 */

	register bblock_p b1,b2;
	register Lindex i,j;
	lset s = b->b_pred;

	for (i = Lfirst(s); i != (Lindex) 0; i = Lnext(i,s)) {
		b1 = (bblock_p) Lelem(i);
		if (Lnrelems(b1->b_succ) != 1) continue;
		for (j = Lfirst(s); j != (Lindex) 0; j = Lnext(j,s)) {
			b2 = (bblock_p) Lelem(j);
			if (b1 != b2 && Lnrelems(b2->b_succ) == 1) {
				if (try_tail(b1,b2)) return TRUE;
			}
		}
	}
	return FALSE;
}



void cj_optimize(void *vp)
{
	/* Perform cross jumping for procedure p.
	 * In case cases a cross-jumping optimization which give
	 * new opportunities for further cross-jumping optimizations.
	 * Hence we repeat the whole process for the entire procedure,
	 * untill we find no further optimizations.
	 */

	proc_p p = vp;
	bblock_p b;
	bool changes = TRUE;

	if (IS_ENTERED_WITH_GTO(p)) return;
	while(changes) {
		changes = FALSE;
		b = p->p_start; 
		while (b != (bblock_p) 0) {
			if (try_pred(b)) {
				changes = TRUE;
			} else {
				b = b->b_next;
			}
		}
	}
}


int main(argc,argv)
	int argc;
	char *argv[];
{
	go(argc,argv,no_action,cj_optimize,no_action,no_action);
	report("cross jumps",Scj);
	exit(0);
}



/******
 * Debugging stuff
 */

extern char em_mnem[]; /* The mnemonics of the EM instructions. */

STATIC void showinstr(lnp) line_p lnp; {

    /* Makes the instruction in `lnp' human readable. Only lines that
     * can occur in expressions that are going to be eliminated are
     * properly handled.
     */
    if (lnp == 0) return;
    if (INSTR(lnp) < sp_fmnem || INSTR(lnp) > sp_lmnem) {
	printf("\t*** ?\n");
	return;
    }

    printf("\t%s", &em_mnem[4 * (INSTR(lnp)-sp_fmnem)]);
    switch (TYPE(lnp)) {
	case OPNO:
	    break;
	case OPSHORT:
	    printf(" %d", SHORT(lnp)); break;
	case OPOBJECT:
	    printf(" %d", OBJ(lnp)->o_id); break;
	case OPOFFSET:
	    printf(" %ld", OFFSET(lnp)); break;
	default:
	    printf(" ?"); break;
    }
    printf("\n");
} /* showinstr */
