/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ A L L O C L I S T . C
 */

#include <em_mnem.h>
#include <em_spec.h>
#include <em_pseu.h>
#include <em_reg.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/def.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "../share/utils.h"
#include "../share/alloc.h"
#include "../share/map.h"
#include "ra.h"
#include "ra_aux.h"
#include "ra_items.h"
#include "ra_allocl.h"
#include "ra_interv.h"

STATIC count_usage(p,item,nrloops,sloopcnt,dloopcnt)
	proc_p p;
	item_p item;
	short  nrloops, sloopcnt[], dloopcnt[];
{
	/* Determine how many times the item is used in every loop.
	 * We maintain a 'static' count and a 'dynamic' count. The dynamic
	 * count estimates the number of times the item is used during
	 * execution, i.e. it gives a higher mark to items used inside
	 * a loop.
	 */

	lset loops;
	loop_p l;
	int i;
	short lev;
	Lindex ui,li;
	time_p u;

	for (i = 0; i <= nrloops; i++) {
		sloopcnt[i] = 0;
		dloopcnt[i] = 0;
	}
	for (ui = Lfirst(item->it_usage); ui != (Lindex) 0;
					  ui = Lnext(ui,item->it_usage)) {
		u = (time_p) Lelem(ui);
		loops = u->t_bblock->b_loops;
		lev = Lnrelems(loops);
		/* set of loops in which this usage of item occurs */
		for (li = Lfirst(loops); li != (Lindex) 0; li=Lnext(li,loops)) {
			l = (loop_p) Lelem(li);
			sloopcnt[l->lp_id]++;
			dloopcnt[l->lp_id] +=
				(IS_FIRM(u->t_bblock) ? loop_scale(lev) : 1);
		}
	}
}



STATIC alloc_p cons_alloc(item,timespan,stat_usecount,
			  dyn_usecount,inits,wholeproc,isloop,iswholeproc)
	item_p item;
	interv_p timespan;
	short stat_usecount,dyn_usecount;
	lset inits;
	alloc_p wholeproc;
	bool isloop,iswholeproc;
{
	alloc_p x;

	x = newalloc();
	x->al_id = ++alloc_id;
	x->al_item = item;
	x->al_timespan = timespan;
	x->al_susecount = stat_usecount;
	x->al_dusecount = dyn_usecount;
	x->al_inits = inits;
	x->al_wholeproc = wholeproc;
	x->al_isloop = isloop;
	x->al_iswholeproc = iswholeproc;
	return x;
}


STATIC insert_alloc(alloc,list_p)
	alloc_p alloc, *list_p;
{
	alloc->al_next = *list_p;
	*list_p = alloc;
}



#define MUST_INIT(i,b)   (i->it_type!=LOCALVAR ||contains(b->B_BEGIN,i->it_lives))
#define MUST_UPDATE(i,b) (i->it_type==LOCALVAR &&contains(b->B_BEGIN,i->it_lives))

STATIC lset loop_inits(lp,item,header)
	loop_p lp;
	item_p item;
	bblock_p header;
{
	/* Build the set of entry points to loop lp where item
	 * must be initialized
	 */

	lset s = Lempty_set();
	if (header != (bblock_p) 0 && MUST_INIT(item,header)) {
		Ladd(header,&s);
	}
	return s;
}



#define IN_LOOP(b)	(Lnrelems(b->b_loops) > 0)

STATIC bblock_p init_point(item)
	item_p item;
{
	/* Find the most appropriate point to initialize any register
	 * containing the item. We want to do the initialization as
	 * late as possible, to allow other items to be put in the
	 * same register, before this initialization. Yet, as we want
	 * to do the initialization only once, it must be done in a
	 * basic block that is a dominator of all points where the
	 * item is used (ultimately in the first block of the procedure).
	 * This basic block should not be part of loop.
	 */

	bblock_p b,dom = 0;
	Lindex ti;
	time_p t;

	for (ti = Lfirst(item->it_usage); ti != (Lindex) 0;
					ti = Lnext(ti,item->it_usage)) {
		t = (time_p) Lelem(ti);
		b = t->t_bblock;
		dom = (dom == (bblock_p) 0 ? b : common_dom(dom,b));
	}
	while (IN_LOOP(dom)) {
		/* Find a dominator of dom (possibly
		 * dom itself) that is outside any loop.
		 */
		dom = dom->b_idom;
	}
	return dom;
}


STATIC add_blocks(b,s,span)
	bblock_p b;
	cset *s;
	interv_p *span;
{
	Lindex pi;

	if (!Cis_elem(b->b_id,*s)) {
		Cadd(b->b_id,s);
		add_interval(b->B_BEGIN,b->B_END,span);
		for (pi = Lfirst(b->b_pred); pi != (Lindex) 0;
				pi = Lnext(pi,b->b_pred)) {
			add_blocks((bblock_p) Lelem(pi),s,span);
		}
	}
}



STATIC whole_lifetime(item,ini_out,span_out)
	item_p item;
	bblock_p *ini_out;
	interv_p *span_out;
{
	/* Find the initialization point and the time_span of the item, if
	 * we put the item in a register during all its uses.
	 */

	bblock_p b, ini = init_point(item);
	cset s = Cempty_set(blength);
	Lindex ti;
	time_p t;
	interv_p span = (interv_p) 0;

	for (ti = Lfirst(item->it_usage); ti != (Lindex) 0;
					ti = Lnext(ti,item->it_usage)) {
		t = (time_p) Lelem(ti);
		b = t->t_bblock;
		add_blocks(b,&s,&span);
	}
	if (!Cis_elem(ini->b_id,s)) {
		add_interval(ini->B_BEGIN,ini->B_END,&span);
	}
	Cdeleteset(s);
	*ini_out = ini;
	*span_out = span;
}




STATIC lset proc_inits(p,item,ini)
	proc_p p;
	item_p item;
	bblock_p ini;
{
	lset s = Lempty_set();

	if (item->it_type != LOCALVAR || item->i_t.it_off >= 0) {
		/* only local variables need not be initialized */
		Ladd(ini, &s);
	}
	return s;
}


STATIC bool updates_needed(lp,item)
	loop_p lp;
	item_p item;
{
	/* See if the value of item is live after the loop has
	 * been exited, i.e. must the item be updated after the loop?
	 */

	Lindex bi,si;
	bblock_p b,s;

	for (bi = Lfirst(lp->LP_BLOCKS); bi != (Lindex) 0;
					bi = Lnext(bi,lp->LP_BLOCKS)) {
		b = (bblock_p) Lelem(bi);
		for (si = Lfirst(b->b_succ); si != (Lindex) 0;
					si = Lnext(si,b->b_succ)) {
			s = (bblock_p) Lelem(si);
			if (!Lis_elem(s,lp->LP_BLOCKS) && MUST_UPDATE(item,s)) {
				return TRUE;
			}
		}
	}
	return FALSE;
}



STATIC short countuses(usage,b)
	lset usage;
	bblock_p b;
{
	short cnt = 0;
	Lindex ti;
	time_p t;

	for (ti = Lfirst(usage); ti != (Lindex) 0; ti = Lnext(ti,usage)) {
		t = (time_p) Lelem(ti);
		if (t->t_bblock == b) cnt++;
	}
	return cnt;
}



STATIC allocs_of_item(p,item,loops,sloopcnt,dloopcnt,alloc_list_p)
	proc_p p;
	item_p item;
	lset loops;
	short *sloopcnt,*dloopcnt; /* dynamic arrays */
	alloc_p *alloc_list_p;
{
	register Lindex li;
	loop_p lp;
	bblock_p header,ini;
	short susecount,dusecount;
	interv_p lt;
	alloc_p wholeproc;

	/* The whole procedure may be used as timespan.
	   The dynamic usecount of a procedure is taken to be the same
	   as its static usecount; this number is not very important, as
	   time-optimziation chooses loops first.
	 */
	whole_lifetime(item,&ini,&lt);
	wholeproc = cons_alloc(item,lt,Lnrelems(item->it_usage),
			       Lnrelems(item->it_usage), proc_inits(p,item,ini),
			       (alloc_p) 0,FALSE,TRUE); 
	insert_alloc(wholeproc, alloc_list_p);
	for (li = Lfirst(loops); li != (Lindex) 0; li = Lnext(li,loops)) {
		lp = (loop_p) Lelem(li);
		if (sloopcnt[lp->lp_id] != 0 && !updates_needed(lp,item)
		    && !((header = lp->LP_HEADER) == (bblock_p) 0 &&
				MUST_INIT(item,lp->lp_entry))) {
			/* Item is used within loop, so consider loop
			 * as a timespan during which item may be put in
			 * a register.
			if ((header = lp->LP_HEADER) == (bblock_p) 0 &&
				MUST_INIT(item,lp->lp_entry)) continue;
			 */
			lt = loop_lifetime(lp);
			susecount = sloopcnt[lp->lp_id];
			dusecount = dloopcnt[lp->lp_id];
			if (MUST_INIT(item,lp->lp_entry)) {
			       /* include header block in timespan */
			       add_interval(header->B_BEGIN,header->B_END,&lt);
			       susecount += countuses(item->it_usage,header);
			} else {
				header = (bblock_p) 0;
			}
			insert_alloc(cons_alloc(item,lt,susecount,dusecount,
				     loop_inits(lp,item,header),wholeproc,
				     TRUE,FALSE),
					  alloc_list_p);
		} else if (sloopcnt[lp->lp_id] != 0) {
			/* I confess: this is a hack.  I didn't expect the
			 * Spanish inquisition.
			 */
			if (wholeproc->al_dusecount < dloopcnt[lp->lp_id])
				wholeproc->al_dusecount = dloopcnt[lp->lp_id];
		}
	}
}



alloc_p build_alloc_list(p,nrloops,itemlist)
	proc_p p;
	short nrloops;
	item_p itemlist;
{
	short *sloopcnt,*dloopcnt; /* dynamic arrays */
	register item_p item;
	alloc_p alloc_list = (alloc_p) 0;

	sloopcnt = (short *) newtable(nrloops);
	dloopcnt = (short *) newtable(nrloops);
	for (item = itemlist; item != (item_p) 0; item = item->it_next) {
		count_usage(p,item,nrloops,sloopcnt,dloopcnt);
		allocs_of_item(p,item,p->p_loops,sloopcnt,dloopcnt,
				&alloc_list);
	}
	oldtable(sloopcnt,nrloops);
	oldtable(dloopcnt,nrloops);
	return alloc_list;
}



build_rivals_graph(alloclist)
	alloc_p alloclist;
{
	/* See which allocations in the list are rivals of each other,
	 * i.e. there is some point of time, falling in both
	 * timespans, at which the items of both allocations are live.
	 * Allocations with the same item (but different timespans) are
	 * not considered to be rivals.
	 * We use an auxiliary data structure "busy" for each allocation,
	 * indicating when the item is live during the timespan of the
	 * allocation.
	 */

	register alloc_p alloc,x;

	for (alloc = alloclist; alloc != (alloc_p) 0; alloc = alloc->al_next) {
		alloc->al_rivals = Cempty_set(alloc_id);
	}
	for (alloc = alloclist; alloc != (alloc_p) 0; alloc = alloc->al_next) {
		alloc->al_busy = 
		   (alloc->al_item->it_type == LOCALVAR ?
			intersect(alloc->al_timespan,alloc->al_item->it_lives) :
			copy_timespan(alloc->al_timespan));
		for (x = alloclist; x != alloc; x = x->al_next) {
			if (x->al_item != alloc->al_item &&
			    not_disjoint(alloc->al_busy,x->al_busy)) {
				Cadd(x->al_id,&alloc->al_rivals);
				Cadd(alloc->al_id,&x->al_rivals);
				if (alloc->al_regtype == x->al_regtype) {
					alloc->al_cntrivals++;
					x->al_cntrivals++;
				}
			}
		}
	}
}
