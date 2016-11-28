/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  C O N T R O L   F L O W
 *
 *  C F _ L O O P . C
 */


#include <stdlib.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/lset.h"
#include "../share/alloc.h"
#include "../share/aux.h"
#include "cf.h"

#define MARK_STRONG(b)	b->b_flags |= BF_STRONG
#define MARK_FIRM(b)	b->b_flags |= BF_FIRM
#define BF_MARK		04
#define MARK(b)		b->b_flags |= BF_MARK
#define MARKED(b)	(b->b_flags&BF_MARK)
#define INSIDE_LOOP(b,lp)  Lis_elem(b,lp->LP_BLOCKS)



/* The algorithm to detect loops that is used here is taken
 * from: Aho & Ullman, Principles of Compiler Design, section 13.1.
 * The algorithm uses the dominator relation between nodes
 * of the control flow graph:
 *  d DOM n => every path from the initial node to n goes through d.
 * The dominator relation is recorded via the immediate dominator tree
 * (b_idom field of bblock struct) from which the dominator relation
 * can be easily computed (see procedure 'dom' below).
 * The algorithm first finds 'back edges'. A back edge is an edge
 * a->b in the flow graph whose head (b) dominates its tail (a).
 * The 'natural loop' of back edge n->d consists of those nodes
 * that can reach n without going through d. These nodes, plus d
 * form the loop.
 * The whole process is rather complex, because different back edges
 * may result in the same loop and because loops may partly overlap
 * each other (without one being nested inside the other).
 */



STATIC bool same_loop(l1,l2)
	loop_p l1,l2;
{
	/* Two loops are the same if:
	 * (1)  they have the same number of basic blocks, and
	 * (2)  the head of the back edge of the first loop
	 *      also is part of the second loop, and
	 * (3)  the tail of the back edge of the first loop
	 *      also is part of the second loop.
	 */

	return (l1->LP_COUNT == l2->LP_COUNT &&
		Lis_elem(l1->lp_entry, l2->LP_BLOCKS) &&
		Lis_elem(l1->lp_end,   l2->LP_BLOCKS));
}



STATIC bool inner_loop(l1,l2)
	loop_p l1,l2;
{
	/* Loop l1 is an inner loop of l2 if:
	 * (1)  the first loop has fewer basic blocks than
	 *      the second one, and
	 * (2)  the head of the back edge of the first loop
	 *      also is part of the second loop, and
	 * (3)  the tail of the back edge of the first loop
	 *      also is part of the second loop.
	 */

	return (l1->LP_COUNT < l2->LP_COUNT &&
		Lis_elem(l1->lp_entry, l2->LP_BLOCKS) &&
		Lis_elem(l1->lp_end,   l2->LP_BLOCKS));
}



STATIC insrt(b,lpb,s_p)
	bblock_p b;
	lset *lpb;
	lset *s_p;
{
	/* Auxiliary routine used by 'natural_loop'.
	 * Note that we use a set rather than a stack,
	 * as Aho & Ullman do.
	 */

	if (!Lis_elem(b,*lpb)) {
		Ladd(b,lpb);
		Ladd(b,s_p);
	}
}


STATIC loop_p natural_loop(d,n)
	bblock_p d,n;
{
	/* Find the basic blocks of the natural loop of the
	 * back edge 'n->d' (i.e. n->d is an edge in the control
	 * flow graph and d dominates n). The natural loop consists
	 * of those blocks which can reach n without going through d.
	 * We find these blocks by finding all predecessors of n,
	 * up to d.
	 */

	loop_p lp;
	bblock_p m;
	lset loopblocks;
	Lindex pi;
	lset s;

	lp = newloop();
	lp->lp_extend = newcflpx();
	lp->lp_entry = d;	/* loop entry block */
	lp->lp_end = n;		/* tail of back edge */
	s = Lempty_set();
	loopblocks = Lempty_set();
	Ladd(d,&loopblocks);
	insrt(n,&loopblocks,&s);
	while ((pi = Lfirst(s)) != (Lindex) 0) {
		m = (bblock_p) Lelem(pi);
		Lremove(m,&s);
		for (pi = Lfirst(m->b_pred); pi != (Lindex) 0;
					pi = Lnext(pi,m->b_pred)) {
			insrt((bblock_p) Lelem(pi),&loopblocks,&s);
		}
	}
	lp->LP_BLOCKS = loopblocks;
	lp->LP_COUNT = Lnrelems(loopblocks);
	return lp;
}


STATIC loop_p org_loop(lp,loops)
	loop_p lp;
	lset   loops;
{
	/* See if the loop lp was already found via another
	 * back edge; if so return this loop; else return 0.
	 */

	register Lindex li;

	for (li = Lfirst(loops); li != (Lindex) 0; li = Lnext(li,loops)) {
		if (same_loop((loop_p) Lelem(li), lp)) {
#ifdef DEBUG
			/* printf("messy loop found\n"); */
#endif
			return (loop_p) Lelem(li);
		}
	}
	return (loop_p) 0;
}



STATIC collapse_loops(loops_p)
	lset *loops_p;
{
	register Lindex li1, li2;
	register loop_p lp1,lp2;

	for (li1 = Lfirst(*loops_p); li1 != (Lindex) 0; li1 = Lnext(li1,*loops_p)) {
		lp1 = (loop_p) Lelem(li1);
		lp1->lp_level = (short) 0;
		/* Lnext(li2,*loops_p) must happen before
		 * Lremove(lp2,loops_p) releases the memory for li2.
		 */
		for (li2 = Lfirst(*loops_p); li2 != (Lindex) 0;) {
			lp2 = (loop_p) Lelem(li2);
			li2 = Lnext(li2,*loops_p);
			if (lp1 != lp2 && lp1->lp_entry == lp2->lp_entry) {
			    Ljoin(lp2->LP_BLOCKS,&lp1->LP_BLOCKS);
			    oldcflpx(lp2->lp_extend);
			    Lremove(lp2,loops_p);
			}
		}
	}
}


STATIC loop_per_block(lp)
	loop_p lp;
{
	bblock_p b;

	/* Update the b_loops sets */

	register Lindex bi;

	for (bi = Lfirst(lp->LP_BLOCKS); bi != (Lindex) 0;
		bi = Lnext(bi,lp->LP_BLOCKS)) {
			b = (bblock_p) Lelem(bi);
			Ladd(lp,&(b->b_loops));
	}
}



STATIC loop_attrib(loops)
	lset loops;
{
	/* Compute several attributes */

	register Lindex li;
	register loop_p lp;
	loop_id lastlpid = 0;

	for (li = Lfirst(loops); li != (Lindex) 0; li = Lnext(li,loops)) {
		lp = (loop_p) Lelem(li);
		lp->lp_id = ++lastlpid;
		loop_per_block(lp);
	}
}



STATIC nest_levels(loops)
	lset loops;
{
	/* Compute the nesting levels of all loops of
	 * the current procedure. For every loop we just count
	 * all loops of which the former is an inner loop.
	 * The running time is quadratic in the number of loops
	 * of the current procedure. As this number tends to be
	 * very small, there is no cause for alarm.
	 */

	register Lindex li1, li2;
	register loop_p lp;

	for (li1 = Lfirst(loops); li1 != (Lindex) 0; li1 = Lnext(li1,loops)) {
		lp = (loop_p) Lelem(li1);
		lp->lp_level = (short) 0;
		for (li2 = Lfirst(loops); li2 != (Lindex) 0;
					li2 = Lnext(li2,loops)) {
			if (inner_loop(lp,(loop_p) Lelem(li2))) {
				lp->lp_level++;
			}
		}
	}
}


STATIC cleanup(loops)
	lset loops;
{
	/* Throw away the LP_BLOCKS sets */

	register Lindex i;

	for (i = Lfirst(loops); i != (Lindex) 0; i = Lnext(i,loops)) {
		Ldeleteset(((loop_p) Lelem(i))->LP_BLOCKS);
	}
}


STATIC bool does_exit(b,lp)
	bblock_p b;
	loop_p   lp;
{
	/* See if b may exit the loop, i.e. if it
	 * has a successor outside the loop
	 */

	Lindex i;

	for (i = Lfirst(b->b_succ); i != (Lindex) 0; i = Lnext(i,b->b_succ)) {
		if (!INSIDE_LOOP(Lelem(i),lp)) return TRUE;
	}
	return FALSE;
}


STATIC mark_succ(b,lp)
	bblock_p b;
	loop_p   lp;
{
	Lindex i;
	bblock_p succ;

	for (i = Lfirst(b->b_succ); i != (Lindex) 0; i = Lnext(i,b->b_succ)) {
		succ = (bblock_p) Lelem(i);
		if (succ != b && succ != lp->lp_entry && INSIDE_LOOP(succ,lp) &&
		   !MARKED(succ)) {
			MARK(succ);
			mark_succ(succ,lp);
		}
	}
}


STATIC void mark_blocks(lp)
	loop_p lp;
{
	/* Mark the strong and firm blocks of a loop.
	 * The last set of blocks consists of the end-block
	 * of the loop (i.e. the head of the back edge
	 * of the natural loop) and its dominators
	 * (including the loop entry block, i.e. the
	 * tail of the back edge).
	 */

	register bblock_p b;

	/* First mark all blocks that are the successor of a
	 * block that may exit the loop (i.e. contains a
	 * -possibly conditional- jump to somewhere outside
	 * the loop.
	 */

	if (lp->LP_MESSY) return; /* messy loops are hopeless cases */
	for (b = lp->lp_entry; b != (bblock_p) 0; b = b->b_next) {
		if (!MARKED(b) && does_exit(b,lp)) {
			mark_succ(b,lp);
		}
	}

	/* Now find all firm blocks. A block is strong
	 * if it is firm and not marked.
	 */

	for (b = lp->lp_end; ; b = b->b_idom) {
		MARK_FIRM(b);
		if (!MARKED(b)) {
			MARK_STRONG(b);
		}
		if (b == lp->lp_entry) break;
	}
}



STATIC mark_loopblocks(loops)
	lset loops;
{
	/* Determine for all loops which basic blocks
	 * of the loop are strong (i.e. are executed
	 * during every iteration) and which blocks are
	 * firm (i.e. executed during every iteration with
	 * the only possible exception of the last one).
	 */
	
	Lindex i;
	loop_p lp;

	for (i = Lfirst(loops); i != (Lindex) 0; i = Lnext(i,loops)) {
		lp = (loop_p) Lelem(i);
		mark_blocks(lp);
	}
}



loop_detection(p)
	proc_p p;
{
	/* Find all natural loops of procedure p. Every loop is
	 * assigned a unique identifying number, a set of basic
	 * blocks, a loop entry block and a nesting level number.
	 * Every basic block is assigned a nesting level number
	 * and a set of loops it is part of.
	 */

	lset loops;  /* the set of all loops */
	loop_p lp,org;
	register bblock_p b;
	bblock_p s;
	Lindex si;

	loops = Lempty_set();
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		for (si = Lfirst(b->b_succ); si != (Lindex) 0;
						si = Lnext(si,b->b_succ)) {
			s = (bblock_p) Lelem(si);
			if (dom(s,b)) {
				/* 'b->s' is a back edge */
				lp = natural_loop(s,b);
				if ((org = org_loop(lp,loops)) == (loop_p) 0) {
				   /* new loop */
				   Ladd(lp,&loops);
				} else {
				   /* Same loop, generated by several back
				    * edges; such a loop is called a messy
				    * loop.
				    */
				   org->LP_MESSY = TRUE;
				   Ldeleteset(lp->LP_BLOCKS);
				   oldcflpx(lp->lp_extend);
				   oldloop(lp);
				}
			}
		}
	}
	collapse_loops(&loops);
	loop_attrib(loops);
	nest_levels(loops);
	mark_loopblocks(loops); /* determine firm and strong blocks */
	cleanup(loops);
	p->p_loops = loops;
}
