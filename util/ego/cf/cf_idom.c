/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  C O N T R O L   F L O W
 *
 *  C F _ I D O M . C
 */


#include "../share/types.h"
#include "../share/debug.h"
#include "../share/lset.h"
#include "../share/alloc.h"
#include "cf.h"


/* The algorithm for finding dominators in a flowgraph
 * that is used here, was developed by Thomas Lengauer
 * and Robert E. Tarjan of Stanford University.
 * The algorithm is described in their article:
 *	    A Fast Algorithm for Finding Dominators
 *	    in a Flowgraph
 *  which was published in:
 *	   ACM Transactions on Programming Languages and Systems,
 *	   Vol. 1, No. 1, July 1979, Pages 121-141.
 */


#define UNREACHABLE(b) (b->B_SEMI == (short) 0)

short	dfs_nr;
bblock_p *vertex;  /* dynamically allocated array */


STATIC void dfs(v)
	bblock_p v;
{
	/* Depth First Search */

	Lindex i;
	bblock_p w;

	v->B_SEMI = ++dfs_nr;
	vertex[dfs_nr] = v->B_LABEL = v;
	v->B_ANCESTOR = (bblock_p) 0;
	for (i = Lfirst(v->b_succ); i != (Lindex) 0; i = Lnext(i,v->b_succ)) {
		w = (bblock_p) Lelem(i);
		if (w->B_SEMI == 0) {
			w->B_PARENT = v;
			dfs(w);
		}
	}
}



STATIC void compress(v)
	bblock_p v;
{
	if (v->B_ANCESTOR->B_ANCESTOR != (bblock_p) 0) {
		compress(v->B_ANCESTOR);
		if (v->B_ANCESTOR->B_LABEL->B_SEMI < v->B_LABEL->B_SEMI) {
			v->B_LABEL = v->B_ANCESTOR->B_LABEL;
		}
		v->B_ANCESTOR = v->B_ANCESTOR->B_ANCESTOR;
	}
}



STATIC bblock_p eval(v)
	bblock_p v;
{
	if (v->B_ANCESTOR == (bblock_p) 0) {
		return v;
	} else {
		compress(v);
		return v->B_LABEL;
	}
}



STATIC void linkblocks(v,w)
	bblock_p v,w;
{
	w->B_ANCESTOR = v;
}



void dominators(bblock_p r, short n)
{
	/* Compute the immediate dominator of every basic
	 * block in the control flow graph rooted by r.
	 */

	register short i;
	Lindex ind, next;
	bblock_p v,w,u;

	dfs_nr = 0;
	vertex = (bblock_p *) newmap(n);
	/* allocate vertex (dynamic array). All remaining
	 * initializations were done by the routine
	 * nextblock of get.c.
	 */
	dfs(r);
	for (i = dfs_nr; i > 1; i--) {
		w = vertex[i];
		for (ind = Lfirst(w->b_pred); ind != (Lindex) 0;
						ind = Lnext(ind,w->b_pred)) {
			v = (bblock_p) Lelem(ind);
			if (UNREACHABLE(v)) continue;
			u = eval(v);
			if (u->B_SEMI < w->B_SEMI) {
				w->B_SEMI = u->B_SEMI;
			}
		}
		Ladd(w,&(vertex[w->B_SEMI]->B_BUCKET));
		linkblocks(w->B_PARENT,w);
		for (ind = Lfirst(w->B_PARENT->B_BUCKET); ind != (Lindex) 0;
							   ind = next) {
			next = Lnext(ind,w->B_PARENT->B_BUCKET);
			v = (bblock_p) Lelem(ind);
			Lremove(v,&w->B_PARENT->B_BUCKET);
			u = eval(v);
			v->b_idom = (u->B_SEMI < v->B_SEMI ? u : w->B_PARENT);
		}
	}
	for (i = 2; i <= dfs_nr; i++) {
		w = vertex[i];
		if (w->b_idom != vertex[w->B_SEMI]) {
			w->b_idom = w->b_idom->b_idom;
		}
	}
	r->b_idom = (bblock_p) 0;
	/* release memory for dynamic array vertex */
	oldmap((void **) vertex,n);
}
