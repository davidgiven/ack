/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 *  L O C A L S . C
 */

#include <stdio.h>
#include <em_mnem.h>
#include <em_spec.h>
#include <em_pseu.h>
#include <em_mes.h>
#include "types.h"
#include "debug.h"
#include "global.h"
#include "lset.h"
#include "cset.h"
#include "def.h"
#include "get.h"
#include "aux.h"
#include "alloc.h"
#include "locals.h"


extern short nrglobals;

short nrlocals;
local_p *locals;  /* dynamic array */

STATIC localvar(off,size,locs,reg,score)
	offset off;
	short  size;
	local_p *locs;
	bool reg;
	offset score;
{
	/* process a reference to a local variable.
	 * A local is characterized by a (offset,size) pair.
	 * We first collect all locals in a list, sorted
	 * by offset. Later we will construct a table
	 * out of this list.
	 */

	local_p lc, x, *prevp;

	prevp = locs;
	for (lc = *locs; lc != (local_p) 0; lc = lc->lc_next) {
		if (lc->lc_off == off && lc->lc_size == size) {
			if (reg) {
				REGVAR(lc); /* register variable */
				lc->lc_score = score;
			}
			return; /* local already present */
		}
		if (lc->lc_off > off) break;
		prevp = &lc->lc_next;
	}
	/* the local was not seen before; create an entry
	 * for it in the list.
	 */
	x = *prevp = newlocal();
	x->lc_off = off;
	x->lc_size = size;
	x->lc_next = lc;
	if (reg) {
		REGVAR(x);
		x->lc_score = score;
	}
}



STATIC check_message(l,locs)
	line_p l;
	local_p *locs;
{
	/* See if l is a register message */

	arg_p arg;

	arg = ARG(l);
	if (aoff(arg,0) == ms_reg && arg->a_next != (arg_p) 0) {
		localvar(aoff(arg,1), (short) aoff(arg,2), locs, TRUE,
			  aoff(arg,4));
	}
}




STATIC check_local_use(l,locs)
	line_p l;
	local_p *locs;
{
	short sz;

	switch(INSTR(l)) {
		case op_lol:
		case op_stl:
		case op_inl:
		case op_del:
		case op_zrl:
			sz = ws;
			break;
		case op_ldl:
		case op_sdl:
			sz = 2 * ws;
			break;
		case op_lil:
		case op_sil:
			sz = ps;
			break;
		case ps_mes:
			check_message(l,locs);
			/* fall through .. */
		default:
			return;
	}
	if (l->l_next && INSTR(l->l_next) == op_nop) {
		/* volatile */
		return;
	}
	localvar(off_set(l),sz,locs,FALSE,(offset) 0);
}


make_localtab(p)
	proc_p p;
{
	/* Make a table of local variables.
	 * This table is used to associate a
	 * unique number with a local. If two
	 * locals overlap (e.g. LDL 4 and LDL 2)
	 * none of them is considered any further,
	 * i.e. we don't compute ud-info for them.
	 */

	local_p prev = 0, next, lc;
	local_p locallist = (local_p) 0;
	short cnt = 0;
	offset x, ill_zone = 0;
	register bblock_p b;
	register line_p   l;

	/* first make a list of all locals used */
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		for (l = b->b_start; l != (line_p) 0; l = l->l_next) {
			check_local_use(l,&locallist);
		}
	}
	/* Now remove overlapping locals, count useful ones on the fly */
	for (lc = locallist; lc != (local_p) 0; lc = lc->lc_next) {
		if (ill_zone != 0 && lc->lc_off < ill_zone) {
			/* this local overlaps with a previous one */
			BADLC(lc);
			if (!IS_BADLC(prev)) {
				BADLC(prev);
				cnt--;
			}
		} else {
			cnt++;
		}
		x = lc->lc_off + lc->lc_size;
		if (ill_zone == 0 || x > ill_zone) {
			ill_zone = x;
		}
		prev = lc;
	}
	/* Now we know how many local variables there are */
	nrlocals = cnt;
	locals = (local_p *) newmap(cnt);
	cnt = 1;
	for (lc = locallist; lc != (local_p) 0; lc = next) {
		next = lc->lc_next;
		if (IS_BADLC(lc)) {
			oldlocal(lc);
		} else {
			locals[cnt++] = lc;
			lc->lc_next = (local_p) 0;
		}
	}
	assert (cnt == nrlocals+1);
}



find_local(off,nr_out,found_out)
	offset off;
	short  *nr_out;
	bool   *found_out;
{
	/* Try to find the local variable at the given
	 * offset. Return its local-number.
	 */

	short v;

	for (v = 1; v <= nrlocals; v++) {
		if (locals[v]->lc_off > off) break;
		if (locals[v]->lc_off == off) {
			*found_out = TRUE;
			*nr_out = v;
			return;
		}
	}
	*found_out = FALSE;
}




var_nr(l,nr_out,found_out)
	line_p l;
	short  *nr_out;
	bool   *found_out;
{
	/* Determine the number of the variable referenced
	 * by EM instruction l.
	 */

	offset off = 0;
	short nr;

	switch(TYPE(l)) {
		case OPOBJECT:
			/* global variable */
			if (OBJ(l)->o_globnr == 0) {
				/* We don't maintain ud-info for this var */
				*found_out = FALSE;
			} else {
				*nr_out = GLOB_TO_VARNR(OBJ(l)->o_globnr);
				*found_out = TRUE;
			}
			return;
		case OPSHORT:
			off = (offset) SHORT(l);
			break;
		case OPOFFSET:
			off = OFFSET(l);
			break;
		default:
			assert(FALSE);
	}
	/* Its's a local variable */
	find_local(off,&nr,found_out);
	if (*found_out) {
		*nr_out = LOC_TO_VARNR(nr);
	}
}
