/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ X F O R M . C
 */

#include "../share/types.h"
#include "../share/debug.h"
#include "../share/def.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/aux.h"
#include "../share/alloc.h"
#include "../../../h/em_mnem.h"
#include "../../../h/em_spec.h"
#include "../../../h/em_pseu.h"
#include "../../../h/em_mes.h"
#include "../../../h/em_ego.h"
#include "../../../h/em_reg.h"
#include "ra.h"
#include "ra_interv.h"
#include "ra_xform.h"
#include "ra_items.h"


/* The replacement table is used to transform instructions that reference
 * items other than local variables (i.e. the address of a local or global
 * variable or a single/double constant; the transformation of an instruction
 * that references a local variable is very simple).
 * The generated code depends on the word and pointer size of the target
 * machine.
 */


struct repl {
	short	r_instr;	/* instruction		*/
	short	r_op;		/* operand		*/
};

/* REGNR,NO and STOP should not equal the wordsize or pointer size
 * of any machine.
 */
#define	REGNR	-3
#define	NO	-2
#define	STOP	-1
#define	PS	0
#define	PS2	1
#define	WS	2
#define	WS2	3

#define LOAD_POINTER	op_nop
#define	BLANK		{0, STOP}

#define NRREPLACEMENTS	13
#define	REPL_LENGTH	3

struct repl repl_tab[NRREPLACEMENTS][REPL_LENGTH] = {
	/* 0 */	{{op_lil, REGNR},	BLANK,		BLANK},
	/* 1 */	{{LOAD_POINTER,REGNR},	{op_loi,PS},	{op_loi,WS}},
	/* 2 */	{{LOAD_POINTER,REGNR},	BLANK,		BLANK},
	/* 3 */ {{LOAD_POINTER,REGNR},	{op_loi,WS2},	BLANK},
	/* 4 */	{{op_sil,REGNR},	BLANK,		BLANK},
	/* 5 */	{{LOAD_POINTER,REGNR},	{op_loi,PS},	{op_sti,WS}},
	/* 6 */	{{LOAD_POINTER,REGNR},	{op_sti,WS2},	BLANK},
	/* 7 */	{{op_lil,REGNR},	{op_inc,NO},	{op_sil,REGNR}},
	/* 8 */	{{op_lil,REGNR},	{op_dec,NO},	{op_sil,REGNR}},
	/* 9 */	{{op_zer,WS},		{op_sil,REGNR},	BLANK},
	/*10 */	{{op_lol,REGNR},	BLANK,		BLANK},
	/*11 */	{{op_ldl,REGNR},	BLANK,		BLANK},
	/*12 */	{{LOAD_POINTER,REGNR},	{op_cai,NO},	BLANK},
};




init_replacements(psize,wsize)
	short psize,wsize;
{
	/* The replacement code to be generated depends on the
	 * wordsize and pointer size of the target machine.
	 * The replacement table is initialized with a description
	 * of which sizes to use. This routine inserts the real sizes.
	 * It also inserts the actual EM instruction to be used
	 * as a 'Load pointer' instruction.
	 */

	register int i,j;
	short load_pointer;
	struct repl *r;

	assert (psize == wsize || psize == 2*wsize);
	load_pointer = (psize == wsize ? op_lol : op_ldl);
	for (i = 0; i < NRREPLACEMENTS; i++) {
		for (j = 0; j < REPL_LENGTH; j++) {
			r = &repl_tab[i][j];
			if (r->r_op == STOP) break;
			if (r->r_instr == LOAD_POINTER) {
				r->r_instr = load_pointer;
			}
			switch (r->r_op) {
				/* initially r_op describes how to compute
				 * the real operand of the instruction. */
				case PS2:
					r->r_op = 2*psize;
					break;
				case PS:
					r->r_op = psize;
					break;
				case WS2:
					r->r_op = 2*wsize;
					break;
				case WS:
					r->r_op = wsize;
					break;
				case NO:
				case REGNR:	/* use offset of dummy local,
						 * will be filled in later.
						 */
					break;
				default: assert(FALSE);
			}
		}
	}
}



STATIC int repl_index(l)
	line_p l;
{
	return itemtab[INSTR(l) - sp_fmnem].id_replindex;
}



STATIC bool is_current(alloc,t)
	alloc_p alloc;
	short t;
{
	/* Is time t part of alloc's timespan? */

	return contains(t,alloc->al_timespan);
}


STATIC match_item(item,l)
	item_p item;
	line_p l;
{
	/* See if the item used by l is the same one as 'item' */
	struct item thisitem;

	fill_item(&thisitem,l);
	if (item->it_type == LOCAL_ADDR && thisitem.it_type == LOCALVAR) {
		/* The usage of a local variable is also considered to
		 * be the usage of the address of that variable.
		 */
		thisitem.it_type = LOCAL_ADDR;
	}
	return item->it_type == thisitem.it_type && same_item(item,&thisitem);
}



STATIC alloc_p find_alloc(alloclist,l,t)
	alloc_p alloclist;
	line_p l;
	short t;
{
	/* See if any of the allocations of the list applies to instruction
	 * l at time t.
	 */

	register alloc_p alloc,m;

	for (alloc = alloclist; alloc != (alloc_p) 0; alloc = alloc->al_next) {
		for (m = alloc; m != (alloc_p) 0; m = m->al_mates) {
			if (is_current(m,t) && match_item(m->al_item,l)) {
				return m;
			}
		}
	}
	return (alloc_p) 0;
}


STATIC replace_line(l,b,list)
	line_p l,list;
	bblock_p b;
{
	if (b->b_start == l) {
		b->b_start = list;
	} else {
		PREV(l)->l_next = list;
	}
	PREV(list) = PREV(l);
	while (list->l_next != (line_p) 0) {
		list = list->l_next;
	}
	list->l_next = l->l_next;
	if (l->l_next != (line_p) 0) {
		PREV(l->l_next) = list;
	}
	oldline(l);
}


STATIC line_p repl_code(lnp,regnr)
	line_p lnp;
	offset  regnr;
{
	line_p head,*q,l,prev = (line_p) 0;
	int i,index;
	struct repl *r;

	q = &head;
	index = repl_index(lnp);
	for (i = 0; i < REPL_LENGTH; i++) {
		r = &repl_tab[index][i];
		if (r->r_op == STOP) break;  /* replacement < REPL_LENGTH */
		switch(r->r_op) {
			case REGNR:
				l = int_line(regnr);
				break;
			case NO:
				l = newline(OPNO);
				break;
			default:
				l = newline(OPSHORT);
				SHORT(l) = r->r_op;
				break;
		}
		*q = l;
		l->l_instr = r->r_instr;
		PREV(l) = prev;
		prev = l;
		q = &l->l_next;
	}
	return head;
}



STATIC apply_alloc(b,l,alloc)
	bblock_p b;
	line_p l;
	alloc_p alloc;
{
	/* 'l' is an EM instruction using an item that will be put in
	 * a register. Generate new code that uses the register instead
	 * of the item.
	 * If the item is a local variable the new code is the same as
	 * the old code, except for the fact that the offset of the
	 * local is changed (it now uses the dummy local that will be
	 * put in a register by the code generator).
	 * If the item is a constant, the new code is a LOL or LDL.
	 * If the item is the address of a local or global variable, things
	 * get more complicated. The new code depends on the instruction
	 * that uses the item (i.e. l). The new code, which may consist of
	 * several instructions, is obtained by consulting a replacement
	 * table.
	 */

	line_p newcode;

	if (alloc->al_item->it_type == LOCALVAR) {
		SHORT(l) = alloc->al_dummy;
	} else {
		newcode = repl_code(l,alloc->al_dummy);
		replace_line(l,b,newcode);
	}
}



STATIC int loaditem_tab[NRITEMTYPES][2] =
{	/* 		WS		2 * WS */
	/*LOCALVAR*/	op_lol,		op_ldl,
	/*LOCAL_ADDR*/	op_lal,		op_lal,
	/*GLOBL_ADDR*/	op_lae,		op_lae,
	/*PROC_ADDR*/	op_lpi,		op_lpi,
	/*CONST*/	op_loc,		op_nop,
	/*DCONST*/	op_nop,		op_ldc
};


STATIC line_p load_item(item)
	item_p item;
{
	/* Generate an EM instruction that loads the item on the stack */

	line_p l;

	switch (item->it_type) {
		case GLOBL_ADDR:
			l = newline(OPOBJECT);
			OBJ(l) = item->i_t.it_obj;
			break;
		case PROC_ADDR:
			l = newline(OPPROC);
			PROC(l) = item->i_t.it_proc;
			break;
		default:
			l = int_line(item->i_t.it_off);
	}
	l->l_instr = loaditem_tab[item->it_type][item->it_size == ws ? 0 : 1];
	assert(l->l_instr != op_nop);
	return l;
}


STATIC line_p store_local(size,off)
	short size;
	offset off;
{
	line_p l = int_line(off);

	l->l_instr = (size == ws ? op_stl : op_sdl);
	return l;
}



STATIC line_p init_place(b)
	bblock_p b;
{

	register line_p l,prev;

	prev = (line_p) 0;
	for (l = b->b_start; l != (line_p) 0; l = l->l_next) {
		switch(INSTR(l)) {
			case ps_mes:
			case ps_pro:
			case op_lab:
				break;
			default:
				return prev;
		}
		prev =l;
	}
	return prev;
}



STATIC append_code(l1,l2,b)
	line_p l1,l2;
	bblock_p b;
{
	/* Append instruction l1 and l2 at begin of block b */

	line_p l;

	DLINK(l1,l2);
	l = init_place(b);
	if (l == (line_p) 0) {
		l2->l_next = b->b_start;
		b->b_start = l1;
		PREV(l1) = (line_p) 0;
	} else {
		l2->l_next = l->l_next;
		DLINK(l,l1);
	}
	if (l2->l_next != (line_p) 0) {
		PREV(l2->l_next) = l2;
	}
}



STATIC emit_init_code(list)
	alloc_p list;
{
	/* Emit initialization code for all packed allocations.
	 * This code looks like "dummy_local := item", e.g.
	 * "LOC 25 ; STL -10" in EM terminology.
	 */

	register alloc_p alloc,m;
	Lindex bi;
	bblock_p b;

	for (alloc = list; alloc != (alloc_p) 0; alloc = alloc->al_next) {
		for (m = alloc; m != (alloc_p) 0; m = m->al_mates) {
			for (bi = Lfirst(m->al_inits); bi != (Lindex) 0;
						  bi = Lnext(bi,m->al_inits)) {
				/* "inits" contains all initialization points */
				b = (bblock_p) Lelem(bi);
				append_code(load_item(m->al_item),
					    store_local(m->al_item->it_size,
							m->al_dummy),
					    b);
			}
		}
	}
}



STATIC emit_mesregs(p,alloclist)
	proc_p  p;
	alloc_p alloclist;
{
	line_p l,m,x;
	alloc_p alloc;


	l = p->p_start->b_start;
	x = l->l_next;
	for (alloc = alloclist; alloc != (alloc_p) 0; alloc = alloc->al_next) {
		m = reg_mes(alloc->al_dummy,alloc->al_item->it_size,
			alloc->al_regtype,INFINITE);
		DLINK(l,m);
		l = m;
	}
	if (x != (line_p) 0) DLINK(l,x); 
}

#define is_mesreg(l)	(INSTR(l) == ps_mes && aoff(ARG(l),0) == ms_reg)



rem_mes(p)
	proc_p p;
{
	register bblock_p b;
	register line_p l,next;
	offset m;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		for (l = b->b_start; l != (line_p) 0; l = next) {
			next = l->l_next;
			if (INSTR(l) == ps_mes
			    && aoff(ARG(l),0) == ms_ego
			    && ((m = aoff(ARG(l),1)) == ego_live
				|| m == ego_dead)) {
				/* remove live/dead messages */
				rm_line(l,b);
			}
		}
	}
}



xform_proc(p,alloclist,nrinstrs,instrmap)
	proc_p p;
	alloc_p alloclist;
	short nrinstrs;
	line_p instrmap[];
{
	/* Transform every instruction of procedure p that uses an item
	 * at a point where the item is kept in a register.
	 */

	register short now = 0;
	register line_p l,next;
	register bblock_p b;
	alloc_p alloc;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		for (l = b->b_start; l != (line_p) 0; l = next) {
			next = l->l_next;
			if (is_mesreg(l) && ARG(l)->a_next != (arg_p) 0 && 
				aoff(ARG(l),4) != INFINITE) {
				/* All register messages for local variables
			         * that were not assigned a register get
				 * their 'count' fields* set to 0.
				 */
				ARG(l)->a_next->a_next->a_next
					->a_next->a_a.a_offset = 0;
			}
			if (is_item(l) && 
			    (alloc = find_alloc(alloclist,l,now))
					     != (alloc_p) 0 ) {
				apply_alloc(b,l,alloc);
			}
			now++;
		}
	}
	emit_init_code(alloclist);
	emit_mesregs(p,alloclist);
	rem_mes(p);
}




STATIC bool always_in_reg(off,allocs,size_out)
	offset off;
	alloc_p allocs;
	short *size_out;
{
	/* See if the local variable with the given offset is stored
	 * in a register during its entire lifetime. As a side effect,
	 * return the size of the local.
	 */

	alloc_p alloc,m;
	item_p item;

	for (alloc = allocs; alloc != (alloc_p) 0; alloc = alloc->al_next) {
		for (m = alloc; m != (alloc_p) 0; m = m->al_mates) {
			item = m->al_item;
			if (m->al_iswholeproc &&
			    item->it_type == LOCALVAR &&
			    item->i_t.it_off == off) {
				*size_out = item->it_size;
				return TRUE;
			}
		}
	}
	return FALSE;
}


rem_locals(p,allocs)
	proc_p p;
	alloc_p allocs;
{
	/* Try to decrease the number of locals of procedure p, by
	 * looking at which locals are always stored in a register.
	 */

	offset nrlocals = p->p_localbytes;
	short size;

	while (nrlocals > 0) {
		/* A local can only be removed if all locals with
		 * higher offsets are removed too.
		 */
		if (always_in_reg(-nrlocals,allocs,&size)) {
			OUTVERBOSE("local %d removed from proc %d\n",
				nrlocals,p->p_id);
			nrlocals -= size;
		} else {
			break;
		}
	}
	p->p_localbytes = nrlocals;
}
rem_formals(p,allocs)
	proc_p p;
	alloc_p allocs;
{
	/* Try to decrease the number of formals of procedure p, by
	 * looking at which formals are always stored in a register.
	 */

	offset nrformals = p->p_nrformals;
	offset off = 0;
	short size;

	if (nrformals == UNKNOWN_SIZE) return;
	while (off < nrformals) {
		if (always_in_reg(off,allocs,&size)) {
			OUTVERBOSE("formal %d removed from proc %d\n",
				off,p->p_id);
			off += size;
		} else {
			break;
		}
	}
	if (nrformals == off) {
		OUTVERBOSE("all formals of procedure %d removed\n",p->p_id,0);
		p->p_nrformals = 0;
	}
}
