/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ P A C K . C
 */

#include "../share/types.h"
#include "../share/debug.h"
#include "../share/def.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "../share/alloc.h"
#include "../share/aux.h"
#include "../../../h/em_reg.h"
#include "ra.h"
#include "ra_aux.h"
#include "ra_interv.h"


short regs_occupied[NRREGTYPES];	/* #occupied registers for reg_pointer,
					 * reg_any etc.
					 */
#define reg_available(t)	(regs_available[t] > regs_occupied[t])

STATIC initregcount()
{
	int t;

	for (t = 0; t < NRREGTYPES; t++) {
		regs_occupied[t] = 0;
	}
}

STATIC alloc_p make_dummy()
{
	alloc_p x;

	x = newalloc();
	/* x->al_profits = 0; */
	return x;
}


STATIC bool fits_in(a,b,cont_item)
	alloc_p a,b;
	bool *cont_item;
{
	/* See if allocation a can be assigned the same register as b.
	 * Both allocations should be of the same register-type.
	 * Note that there may be several other allocations (mates) assigned to
	 * the same register as b. A new candidate (i.e. 'a') is only
	 * allowed to join them if it is not the rival of any resident
	 * allocation.
	 */

	*cont_item = FALSE;
	if (a->al_regtype == b->al_regtype) {
		while (b != (alloc_p) 0) {
			if (Cis_elem(a->al_id,b->al_rivals)) break;
			b = b->al_mates;
			if (b != (alloc_p) 0 && a->al_item == b->al_item) {
				*cont_item = TRUE;
			}
		}
	}
	return b == (alloc_p) 0;
}


STATIC alloc_p find_fitting_alloc(alloc,packed)
	alloc_p alloc,packed;
{
	/* Try to find and already packed allocation that is assigned
	 * a register that may also be used for alloc.
	 * We prefer allocations that have the same item as alloc.
	 */

	register alloc_p x;
	alloc_p cand = (alloc_p) 0;
	bool cont_item;

	for (x = packed->al_next; x != (alloc_p) 0; x = x->al_next) {
		if (fits_in(alloc,x,&cont_item)) {
			cand = x;
			if (cont_item) break;
		}
	}
	return cand;
}


STATIC bool room_for(alloc,packed)
	alloc_p alloc,packed;
{
	/* See if there is any register available for alloc */

	return reg_available(alloc->al_regtype) ||
		(find_fitting_alloc(alloc,packed) != (alloc_p) 0);
}



STATIC alloc_p best_alloc(unpacked,packed,time_opt)
	alloc_p unpacked,packed;
	bool time_opt;	/* now unused */
{
	/* Find the next best candidate */

	register alloc_p x,best;

	best = unpacked; /* dummy */

	for (x = unpacked->al_next; x != (alloc_p) 0; x = x->al_next) {
		if (x->al_profits > best->al_profits &&
		    room_for(x,packed)) {
			best = x;
		}
	}
	return (best == unpacked ? (alloc_p) 0 : best);
}




STATIC alloc_p choose_location(alloc,packed,p)
	alloc_p alloc,packed;
	proc_p p;
{
	/* Decide in which register to put alloc */

	alloc_p fit;
	offset dum;

	fit = find_fitting_alloc(alloc,packed);
	if (fit == (alloc_p) 0) {
		/* Take a brand new register; allocate a dummy local for it */
		alloc->al_regnr = regs_occupied[alloc->al_regtype]++;
		dum = tmplocal(p,(offset) alloc->al_item->it_size);
		alloc->al_dummy = dum;
	} else {
		alloc->al_regnr = fit->al_regnr;
		alloc->al_dummy = fit->al_dummy;
	}
	return fit;
}



STATIC update_lists(alloc,unpacked,packed,fit)
	alloc_p alloc,unpacked,packed,fit;
{
	/* 'alloc' has been granted a register; move it from the 'unpacked'
	 * list to the 'packed' list. Also remove any allocation from 'unpacked'
	 * having:
	 *  1. the same item as 'alloc' and
	 *  2. a timespan that overlaps the timespan of alloc.
	 */

	register alloc_p x,q,next;

	q = unpacked; /* dummy element at head of list */
	for (x = unpacked->al_next; x != (alloc_p) 0; x = next) {
		next = x->al_next;
		if (x->al_item == alloc->al_item &&
		    not_disjoint(x->al_timespan, alloc->al_timespan)) {
			/* this code kills two birds with one stone;
			 * x is either an overlapping allocation or
			 * alloc itself!
			 */
			q->al_next = x->al_next;
			if (x == alloc) {
				if (fit == (alloc_p) 0) {
					x->al_next = packed->al_next;
					packed->al_next = x;
				} else {
					x->al_mates = fit->al_mates;
					fit->al_mates = x;
					x->al_next = (alloc_p) 0;
				}
			}
		} else {
			q = x;
		}
	}
}



STATIC short cum_profits(alloc)
	alloc_p alloc;
{
	/* Add the profits of all allocations packed in the same
	 * register as alloc (i.e. alloc and all its 'mates').
	 */
	
	alloc_p m;
	short sum = 0;

	for (m = alloc; m != (alloc_p) 0; m = m->al_mates) {
		sum += m->al_profits;
	}
	return sum;
}



STATIC alloc_p best_cumprofits(list,x_out,prev_out)
	alloc_p list, *x_out, *prev_out;
{
	/* Find the allocation with the best cummulative profits */

	register alloc_p x,prev,best_prev;
	short best = 0, cum;

	prev = list;
	for (x = list->al_next; x != (alloc_p) 0; x = x->al_next) {
		cum = cum_profits(x);
		if (cum > best) {
			best = cum;
			best_prev = prev;
		}
		prev = x;
	}
	if (best == 0) {
		*x_out = (alloc_p) 0;
	} else {
		*x_out = best_prev->al_next;
		*prev_out = best_prev;
	}
}



STATIC account_regsave(packed,unpacked)
	alloc_p packed,unpacked;
{
	/* After all packing has been done, we check for every allocated
	 * register whether it is really advantageous to use this
	 * register. It may be possible that the cost of saving
	 * and restoring the register are higher than the profits of all
	 * allocations packed in the register. If so, we simply remove
	 * all these allocations.
	 * The cost of saving/restoring one extra register may depend on 
	 * the number of registers already saved.
	 */

	alloc_p x,prev,checked;
	short time,space;
	short tot_cost = 0,diff;

	initregcount();
	checked = make_dummy();
	while (TRUE) {
		best_cumprofits(packed,&x,&prev);
		if (x == (alloc_p) 0) break;
		regs_occupied[x->al_regtype]++;
		regsave_cost(regs_occupied,&time,&space);
		diff = add_timespace(time,space) - tot_cost;
		if (diff < cum_profits(x)) {
			/* x is o.k. */
			prev->al_next = x->al_next;
			x->al_next = checked->al_next;
			checked->al_next = x;
			tot_cost += diff;
		} else {
			break;
		}
	}
	/* Now every allocation in 'packed' does not pay off, so
	 * it is moved to unpacked, indicating it will not be assigned
	 * a register.
	 */
	for (x = unpacked; x->al_next != (alloc_p) 0; x = x->al_next);
	x->al_next = packed->al_next;
	packed->al_next = checked->al_next;
	oldalloc(checked);
}



STATIC bool in_single_reg(item,packed)
	item_p item;
	alloc_p packed;
{
	/* See if item is allocated in only one register (i.e. not in
	 * several different registers during several parts of its lifetime.
	 */

	register alloc_p x,m;
	bool seen = FALSE;

	for (x = packed->al_next; x != (alloc_p) 0; x = x->al_next) {
		for ( m = x; m != (alloc_p) 0; m = m->al_mates) {
			if (m->al_item == item) {
				if (seen) return FALSE;
				seen = TRUE;
				break;
			}
		}
	}
	return TRUE;
}



STATIC alloc_p find_prev(alloc,list)
	alloc_p alloc,list;
{
	register alloc_p x;

	assert ( alloc != (alloc_p) 0);
	for (x = list; x->al_next != alloc ; x = x->al_next)
		assert(x != (alloc_p) 0);
	return x;
}


/* If an item is always put in the same register during different loops,
 * we try to put it in that register during the whole procedure.
 * The profits of the whole-procedure allocation are updated to prevent
 * account_regsave from rejecting it.
 */

STATIC repl_allocs(new,old,packed)
	alloc_p new,old,packed;
{
	alloc_p x,next,prev,*p;
	short prof = 0;

	new->al_regnr = old->al_regnr;
	new->al_dummy = old->al_dummy;
	prev = find_prev(old,packed);
	new->al_next = old->al_next;
	old->al_next = (alloc_p) 0;
	prev->al_next = new;
	new->al_mates = old;
	p = &new->al_mates;
	for (x = old; x != (alloc_p) 0; x = next) {
		next = x->al_mates;
		if (x->al_item == new->al_item) {
			prof += x->al_profits;
			*p = next;
			oldalloc(x);
		} else {
			p = &x->al_mates;
		}
	}
	new->al_profits = prof;
}



STATIC assemble_allocs(packed)
	alloc_p packed;
{
	register alloc_p x,m,next;
	alloc_p e;
	bool voidb;

	for (x = packed->al_next; x != (alloc_p) 0; x = next) {
		next = x->al_next;
		for ( m = x; m != (alloc_p) 0; m = m->al_mates) {
			if (in_single_reg(m->al_item,packed) &&
			    (e = m->al_wholeproc) != (alloc_p) 0 &&
			    e->al_profits > 0 &&
			    fits_in(e,x,&voidb)) {
				repl_allocs(e,x,packed);
				break;
			}
		}
	}
}

pack(alloclist,time_opt,packed_out,not_packed_out,p)
	alloc_p alloclist, *packed_out,*not_packed_out;
	bool time_opt;
	proc_p p;
{
	/* This is the packing system. It decides which allations
	 * to grant a register.
	 * We use two lists: packed (for allocations that are assigned a
	 * register) and unpacked (allocations not yet assigned a register).
	 * The packed list is in fact '2-dimensional': the al_next field is
	 * used to link allations that are assigned different registers;
	 * the al_mates field links allocations that are assigned to
	 * the same registers (i.e. these allocations fit together).
	 */

	register alloc_p x;
	alloc_p packed,unpacked,fit;

	initregcount();
	packed = make_dummy();
	unpacked = make_dummy();
	unpacked->al_next = alloclist;
	while ((x = best_alloc(unpacked,packed,time_opt)) != (alloc_p) 0) {
		fit = choose_location(x,packed,p);
		update_lists(x,unpacked,packed,fit);
	}
	assemble_allocs(packed);
	account_regsave(packed,unpacked); 
	/* remove allocations that don't pay off against register
	 * save/restore costs.
	 */
	*packed_out = packed->al_next;
	*not_packed_out = unpacked->al_next;
	oldalloc(packed);
	oldalloc(unpacked);
}
