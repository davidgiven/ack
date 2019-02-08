/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* S T R E N G T H   R E D U C T I O N
 *
 * S R _ X F O R M . C
 *
 */



#include <stdio.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include <em_spec.h>
#include "../share/types.h"
#include "sr.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/alloc.h"
#include "../share/def.h"
#include "../share/get.h"
#include "sr_aux.h"
#include "../share/lset.h"
#include "../share/utils.h"
#include "sr_xform.h"

/* Transformations on EM texts */

line_p move_pointer(tmp,dir)
	offset tmp;
	int    dir;
{
	/* Generate EM code to load/store a pointer variable
	 * onto/from the stack, depending on dir(ection).
	 * We accept all kinds of pointer sizes.
	 */

	line_p l;

	l = int_line(tmp);
	if (ps == ws) {
		/* pointer fits in a word */
		l->l_instr = (dir == LOAD ? op_lol : op_stl);
	} else {
		if (ps == 2 * ws) {
			/* pointer fits in a double word */
			l->l_instr = (dir == LOAD ? op_ldl : op_sdl);
		} else {
			/* very large pointer size, generate code:
			 * LAL tmp ; LOI/STI ps */
			l->l_instr = op_lal;
			l->l_next = newline(OPSHORT);
			SHORT(l->l_next) = ps;
			l->l_next->l_instr =
			   (dir == LOAD ? op_loi : op_sti);
			PREV(l->l_next) = l;
		}
	}
	return l;
}



/* make_header */

STATIC copy_loops(b1,b2,except)
	bblock_p b1,b2;
	loop_p except;
{
	/* Copy the loopset of b2 to b1, except for 'except' */

	Lindex i;
	loop_p lp;
	for (i = Lfirst(b2->b_loops); i != (Lindex) 0;
	     i = Lnext(i,b2->b_loops)) {
		lp = (loop_p) Lelem(i);
		if (lp != except) {
			Ladd(lp,&b1->b_loops);
		}
	}
}


STATIC lab_id label(b)
	bblock_p b;
{
	/* Find the label at the head of block b. If there is
	 * no such label yet, create one.
	 */

	line_p l;

	if (b->b_start && INSTR(b->b_start) == op_lab) {
		return INSTRLAB(b->b_start);
	}
	/* The block has no label yet. */
	l = newline(OPINSTRLAB);
	l->l_instr = op_lab;
	INSTRLAB(l) = freshlabel();
	if (b->b_start) {
		DLINK(l,b->b_start); /* doubly link them */
	}
	b->b_start = l;
	return INSTRLAB(l);
}


STATIC adjust_jump(newtarg,oldtarg,c)
	bblock_p newtarg,oldtarg,c;
{
	/* If the last instruction of c is a jump to the
	 * old target, then change it into a jump to the
	 * start of the new target.
	 */

	line_p l = last_instr(c);

	assert(l != (line_p) 0);

	if (INSTR(oldtarg->b_start) == op_lab) {
		/* If old target has no label, it cannot be jumped to */
		if (TYPE(l) == OPINSTRLAB &&
		    INSTRLAB(l) == INSTRLAB(oldtarg->b_start)) {
			INSTRLAB(l) = label(newtarg);
		}
	}

	if (c->b_next == oldtarg && INSTR(l) != op_bra) {
		line_p new = newline(OPINSTRLAB);

		INSTRLAB(new) = label(newtarg);
		new->l_instr = op_bra;
		DLINK(l, new);
	}
}


void
make_header(lp)
	loop_p lp;
{
	/* Make sure that the loop has a header block, i.e. a block
	 * has the loop entry block as its only successor and
	 * that dominates the loop entry block.
	 * If there is no header yet, create one.
	 */

	bblock_p b,c,entry;
	Lindex i,next;

	if (lp->LP_HEADER != (bblock_p) 0) return;
	OUTTRACE("creating a new header block",0);
	/* The loop has no header yet. The main problem is to
	 * keep all relations (SUCC, PRED, NEXT, IDOM, LOOPS)
	 * up to date.
	 */
	b = freshblock(); /* new block with new b_id */
	entry = lp->lp_entry;

	/* update succ/pred. Also take care that any jump from outside
	 * the loop to the entry block now goes to b.
	 */

	b->b_succ = Lempty_set();
	b->b_pred = Lempty_set();

	for (i = Lfirst(entry->b_pred); i != (Lindex) 0; i = next ) {
		next = Lnext(i,entry->b_pred);
		c = (bblock_p) Lelem(i);
		/* c is a predecessor of the entry block */
		if (!Lis_elem(c,lp->LP_BLOCKS)) {
			/* c is outside the loop */
			Lremove(c,&entry->b_pred);
			Lremove(entry,&c->b_succ);
			Ladd(b,&c->b_succ);
			Ladd(c,&b->b_pred);
			adjust_jump(b,entry,c);
		}
	}
	assert(lp->LP_INSTR == 0);
	lp->LP_INSTR = b->b_start;
	Ladd(b,&entry->b_pred);
	Ladd(entry,&b->b_succ);
	/* put header block at end of procedure */
	for (c = curproc->p_start; c->b_next != 0; c = c->b_next);
	c->b_next = b;
	/* b->b_next = 0; */
	copy_loops(b,entry,lp);
	b->b_idom = entry->b_idom;
	entry->b_idom = b;
	lp->LP_HEADER = b;
}
