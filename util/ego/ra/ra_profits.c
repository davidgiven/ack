/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ P R O F I T S . C
 */

#include <stdio.h>
#include <em_reg.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/lset.h"
#include "../share/global.h"
#include "ra.h"
#include "ra_aux.h"
#include "ra_profits.h"

STATIC bool test_cond(cond,val)
	short cond;
	offset val;
{
	switch(cond) {
		case DEFAULT:
			return TRUE;
		case FITBYTE:
			return val >= -128 && val < 128;
		case IN_0_63:
			return val >= 0 && val <= 63;
		case IN_0_8:
			return val >= 0 && val <= 8;
	}
}

STATIC short map_value(tab,val,time)
	struct cond_tab tab[];
	offset val;
	bool time;
{
	cond_p p;

	for (p = &tab[0]; ; p++) {
		if (test_cond(p->mc_cond,val)) {
			return (time ? p->mc_tval : p->mc_sval);
		}
	}
}


STATIC short index_value(tab,n,time)
	struct cond_tab tab[];
	short n;
	bool time;
{
	cond_p p;

	p = &tab[n]; 
	return (time ? p->mc_tval : p->mc_sval);
}


allocscore(itemtyp,localtyp,size,off,totyp,time_out,space_out) 
	short itemtyp, localtyp,totyp,size;
	offset off;
	short *time_out, *space_out;
{
	cond_p m = (cond_p) 0;

	if (localtyp == reg_loop) localtyp = reg_any;
	if (size == ws || size ==ps && totyp == reg_pointer) {
		switch(itemtyp) {
		   case LOCALVAR:
			m = alocaltab[localtyp][totyp];
			break;
		   case LOCAL_ADDR:
			if (use_any_as_pointer || totyp == reg_pointer)
				m = alocaddrtab[localtyp][totyp];
			break;
		   case CONST:
			m = aconsttab;
			break;
		   case DCONST:
			m = aconsttab;
			break;
		   case GLOBL_ADDR:
			if (use_any_as_pointer || totyp == reg_pointer)
				m = aglobaltab;
			break;
		   case PROC_ADDR:
			if (use_any_as_pointer || totyp == reg_pointer)
				m = aproctab;
			break;
		}
	}
	*time_out = (m == (cond_p) 0 ? -1 : map_value(m,off,TRUE));
	*space_out = (m == (cond_p) 0 ? -1 : map_value(m,off,FALSE));
	/*
	fprintf(stderr,"itemtyp = %d, localtyp = %d off = %ld\n",itemtyp,localtyp,off);
	fprintf(stderr,"ALLOCSCORE = (%d,%d)\n",*time_out,*space_out);
	*/
}

opening_cost(itemtyp,localtyp,off,time_out,space_out) 
	short itemtyp, localtyp;
	offset off;
	short *time_out, *space_out;
{
	cond_p m;

	if (localtyp == reg_loop) localtyp = reg_any;
	switch(itemtyp) {
	   case LOCALVAR:
		m = olocaltab[localtyp];
		break;
	   case LOCAL_ADDR:
		m = olocaddrtab[localtyp];
		break;
	   case CONST:
		m = oconsttab;
		break;
	   case DCONST:
		m = oconsttab;
		break;
	   case GLOBL_ADDR:
		m = oglobaltab;
		break;
	   case PROC_ADDR:
		m = oproctab;
		break;
	}
	*time_out = (m == (cond_p) 0 ? 1000 : map_value(m,off,TRUE));
	*space_out = (m == (cond_p) 0 ? 1000 : map_value(m,off,FALSE));
	/*
	fprintf(stderr,"itemtyp = %d, localtyp = %d off = %ld\n",itemtyp,localtyp,off);
	fprintf(stderr,"OPEN_COST = (%d,%d)\n",*time_out,*space_out);
	*/
}




regsave_cost(regs,time_out,space_out)
	short regs[], *time_out, *space_out;
{
	/* Estimate the costs of saving and restoring the registers
	 * The array regs contains the number of registers of every
	 * possible type.
	 */

	short n = regs[reg_any] + regs[reg_pointer] + regs[reg_float]; 
	/* #registers */

	*time_out = index_value(regsav_cost,n,TRUE);
	*space_out = index_value(regsav_cost,n,FALSE);
	/*
	fprintf(stderr,"REGSAVE COST, n=%d, (%d,%d)\n",n,*time_out,*space_out);
	*/
}



STATIC short dyn_inits(inits)
	lset inits;
{
	Lindex i;
	short sum = 0;
	bblock_p b;

	for (i = Lfirst(inits); i != (Lindex) 0; i = Lnext(i,inits)) {
		b = (bblock_p) Lelem(i);
		sum += loop_scale(Lnrelems(b->b_loops));
	}
	return sum;
}



compute_profits(alloclist,time_opt)
	alloc_p alloclist;
	bool time_opt;
{
	/* Compute the profits attribute of every allocation.
	 * If the item of an allocation may be put in several types
	 * of register, we choose only the most advanteagous one.
	 */

	register alloc_p alloc;
	short s,t,rtyp,maxsc;
	item_p item;
	short time,space,sc;
	short otime,ospace;
	offset off;
	short cnt,nr_inits;

	for (alloc = alloclist; alloc != (alloc_p) 0; alloc = alloc->al_next) {
		maxsc = 0;
		item = alloc->al_item;
		switch(item->it_type) {
			case LOCALVAR:
			case LOCAL_ADDR:
			case CONST:
			case DCONST:
				off = item->i_t.it_off;
				break;
			default:
				off = 0;
		}
		for (rtyp = item->it_regtype; ; rtyp = reg_any) {
			allocscore( 	item->it_type,
					item->it_regtype,
					item->it_size,
					off,
					rtyp,
					&time,
					&space);
			opening_cost( 	item->it_type,
					item->it_regtype,
					off,
					&otime,
					&ospace);
			nr_inits = Lnrelems(alloc->al_inits);
			s = alloc->al_susecount * space - 
				nr_inits*ospace;
#ifdef __STRANGE__
			if (!alloc->al_isloop && nr_inits > 0) {
				/* might lead to increase of execution time */
				cnt = 0;
			} else
#endif
			{
				cnt = alloc->al_dusecount;
			}
			t = cnt * time - dyn_inits(alloc->al_inits) * otime;
			sc = (time_opt ? t : s);
			/*
			fprintf(stderr, "cnt: %d time: %d otime: %d t: %d s: %d score: %d\n", cnt, time, otime, t, s, sc);
			*/
			if (sc > maxsc) {
				maxsc = sc;
				alloc->al_regtype = rtyp;
				alloc->al_profits = sc;
			}
			if (rtyp == reg_any) break;
		}
	}
}
