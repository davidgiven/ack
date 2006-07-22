/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* S T A C K   P O L L U T I O N
 *
 * S P . C
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
#include "../share/aux.h"
#include "../share/go.h"
#include "../share/stack_chg.h"


/* Stack pollution throws away the ASP instructions after a procedure call.
 * This saves a lot of code, at the cost of some extra stack space.
 * ASPs that are part of a loop are not removed.
 */

#define BF_MARK		04
#define MARK(b)		b->b_flags |= BF_MARK
#define NOT_MARKED(b)	(!(b->b_flags&BF_MARK))
#define IN_LOOP(b)	(Lnrelems(b->b_loops) > 0)

STATIC int Ssp;  /* number of optimizations */

/* According to the EM definition, the stack must be cleaned up
 * before any return. However, for some backends it causes no harm
 * if the stack is not cleaned up. If so, we can do Stack Pollution
 * more globally.
 */

STATIC int globl_sp_allowed;


#define IS_ASP(l)	(INSTR(l) == op_asp && TYPE(l) == OPSHORT && SHORT(l) > 0)


STATIC sp_machinit(f)
	FILE *f;
{
	/* Read target machine dependent information for this phase */
	char s[100];

	for (;;) {
		while(getc(f) != '\n');
		fscanf(f,"%s",s);
		if (strcmp(s,"%%SP") == 0)break;
	}
	fscanf(f,"%d",&globl_sp_allowed);
}
comb_asps(l1,l2,b)
	line_p l1,l2;
	bblock_p b;
{
	assert(INSTR(l1) == op_asp);
	assert(INSTR(l2) == op_asp);
	assert(TYPE(l1) == OPSHORT);
	assert(TYPE(l2) == OPSHORT);

	SHORT(l2) += SHORT(l1);
	rm_line(l1,b);
}
	



stack_pollution(b)
	bblock_p b;
{
	/* For every pair of successive ASP instructions in basic
	 * block b, try to combine the two into one ASP.
	 */

	register line_p l;
	line_p asp,next = b->b_start;
	bool asp_seen = FALSE;
	int stack_diff,pop,push;
	bool ok;

	do {
		stack_diff = 0;
		for (l = next; l != (line_p) 0; l = next)  {
			next = l->l_next;
			if (IS_ASP(l)) break;
			if (asp_seen) {
				if (INSTR(l) == op_ret) {
					stack_diff -= SHORT(l);
				} else {
					line_change(l,&ok,&pop,&push);
					if (!ok || (stack_diff -= pop) < 0) {
						/* can't eliminate last ASP */
						asp_seen = FALSE;
					} else {
						stack_diff += push;
					}
				}
			}
		}
		if (asp_seen) {
			if (l == (line_p) 0) {
				/* last asp of basic block */
				if (globl_sp_allowed && 
				    NOT_MARKED(b) && !IN_LOOP(b)) {
					Ssp++;
					rm_line(asp,b);
				}
			} else {
				/* try to combine with previous asp */
				if (SHORT(l) == stack_diff) {
					Ssp++;
					comb_asps(asp,l,b);
				}
			}
		}
		asp = l;
		asp_seen = TRUE;  /* use new ASP for next try! */
	} while (asp != (line_p) 0);
}

STATIC bool block_save(b)
	bblock_p b;
{

	register line_p l;
	int stack_diff,pop,push;
	bool ok;

	stack_diff = 0;
	for (l = b->b_start; l != (line_p) 0; l = l->l_next)  {
		if (INSTR(l) == op_ret) {
			stack_diff -= SHORT(l);
			break;
		}
		line_change(l,&ok,&pop,&push);
		/* printf("instr %d, pop %d,push %d,ok %d\n",INSTR(l),pop,push,ok);  */
		if (!ok || (stack_diff -= pop) < 0) {
			return FALSE;
		} else {
			stack_diff += push;
		}
	}
	return stack_diff >= 0;
}



STATIC mark_pred(b)
	bblock_p b;
{
	Lindex i;
	bblock_p x;

	for (i = Lfirst(b->b_pred); i != (Lindex) 0; i = Lnext(i,b->b_pred)) {
		x = (bblock_p) Lelem(i);
		if (NOT_MARKED(x)) {
			MARK(x);
			mark_pred(x);
		}
	}
}





STATIC mark_unsave_blocks(p)
	proc_p p;
{
	register bblock_p b;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		if (NOT_MARKED(b) && !block_save(b)) {
			MARK(b);
			mark_pred(b);
		}
	}
}


sp_optimize(p)
	proc_p p;
{
	register bblock_p b;

	if (IS_ENTERED_WITH_GTO(p)) return;
	mark_unsave_blocks(p);
	for (b = p->p_start; b != 0; b = b->b_next) {
		stack_pollution(b);
	}
}




main(argc,argv)
	int argc;
	char *argv[];
{
	go(argc,argv,no_action,sp_optimize,sp_machinit,no_action);
	report("stack adjustments deleted",Ssp);
	exit(0);
}




/***** DEBUGGING:

debug_stack_pollution(p)
	proc_p p;
{
	register bblock_p b;
	register line_p l;
	int lcnt,aspcnt,instr;

	for (b = p->p_start; b != 0; b = b->b_next) {
		lcnt = 0; aspcnt = 0;
		for (l = b->b_start; l != 0; l= l->l_next) {
			instr = INSTR(l);
			if (instr >= sp_fmnem && instr <= sp_lmnem) {
				lcnt++;
				if (instr == op_asp && off_set(l) > 0) {
					aspcnt++;
				}
			}
		}
		printf("%d\t%d\n",aspcnt,lcnt);
	}
}

*/
