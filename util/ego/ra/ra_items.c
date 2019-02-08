/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  R E G I S T E R   A L L O C A T I O N
 *
 *  R A _ I T E M S . C
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
#include "../share/utils.h"
#include "../share/alloc.h"
#include "ra.h"
#include "ra_aux.h"
#include "ra_items.h"


#include "itemtab.h"
/* Maps EM mnemonics onto item types, e.g. op_lol -> LOCALVAR, op_ldc->DCONST,
 * generated from em_mmen.h and itemtab.src files.
 */

#define SMALL_CONSTANT(c)	(c >= 0 && c <= 8)
/* prevent small constants from being put in a register */


clean_tab(items)
	item_p items[];
{
	int t;

	for (t = 0; t < NRITEMTYPES;t++) {
		items[t] = (item_p) 0;
	}
}




short item_type(l)
	line_p l;
{
	int instr = INSTR(l);
	int t;

	if (instr < sp_fmnem || instr > sp_lmnem) return NO_ITEM;
	t =  itemtab[instr - sp_fmnem].id_type;
	if (t == CONST && SMALL_CONSTANT(off_set(l))) return NO_ITEM;
	return t;
}



bool is_item(l)
	line_p l;
{
	return item_type(l) != NO_ITEM;
}


item_p item_of(off,items)
	offset off;
	item_p items[];
{
	register item_p x;

	for (x = items[LOCALVAR]; x != (item_p) 0; x = x->it_next) {
		if (off == x->i_t.it_off) {
			if (!x->it_desirable) break; 
					/* don't put this item in reg */
			return x;
		}
	}
	return (item_p) 0;
}



fill_item(item,l)
	item_p item;
	line_p l;
{
	item->it_type = item_type(l); 
	item->it_desirable = TRUE;
	switch(item->it_type) {
		case GLOBL_ADDR:
			item->i_t.it_obj = OBJ(l);
			break;
		case PROC_ADDR:
			item->i_t.it_proc = PROC(l);
			break;
		default:
			item->i_t.it_off = off_set(l);
	}
}



STATIC bool desirable(l)
	line_p l;
{
	/* See if it is really desirable to put the item of line l
	 * in a register. We do not put an item in a register if it
	 * is used as 'address of array descriptor' of an array
	 * instruction.
	*/

	if (l->l_next != (line_p) 0) {
		switch(INSTR(l->l_next)) {
			case op_aar:
			case op_lar:
			case op_sar:
				return FALSE;
		}
	}
	return TRUE;
}



STATIC int cmp_items(a,b)
	item_p a,b;
{
	/* This routine defines the <, = and > relations between items,
	 * used to sort them for fast lookup.
	 */

	offset n1,n2;

	switch(a->it_type) {
		case GLOBL_ADDR:
			assert(b->it_type == GLOBL_ADDR);
			n1 = (offset) a->i_t.it_obj->o_id;
			n2 = (offset) b->i_t.it_obj->o_id;
			break;
		case PROC_ADDR:
			assert(b->it_type == PROC_ADDR);
			n1 = (offset) a->i_t.it_proc->p_id;
			n2 = (offset) b->i_t.it_proc->p_id;
			break;
		default:
			n1 = a->i_t.it_off;
			n2 = b->i_t.it_off;
	}
	return (n1 == n2 ? 0 : (n1 > n2 ? 1 : -1));
}



bool same_item(a,b)
	item_p a,b;
{
	return cmp_items(a,b) == 0;
}


STATIC bool lt_item(a,b)
	item_p a,b;
{
	return cmp_items(a,b) == -1;
}



/* build_itemlist()
 *
 * Build a list of all items used in the current procedure. An item
 * is anything that can be put in a register (a local variable, a constant,
 * the address of a local or global variable).
 * For each type of item we use a sorted list containing all items of
 * that type found so far.
 * A local variable is only considered to be an item if there is a
 * register message for it (indicating it is never accessed indirectly).
 * For each item, we keep track of all places where it is used
 * (either fetched or stored into). The usage of a local variable is also
 * considered to be a usage of its address.
 */



static item_p items[NRITEMTYPES];  /* items[i] points to the list of type i */



STATIC short reg_type(item)
	item_p item;
{
	/* See which type of register the item should best be assigned to */

	switch(item->it_type) {
		case LOCALVAR:
			return regv_type(item->i_t.it_off);
			/* use type mentioned in reg. message for local */
		case LOCAL_ADDR:
		case GLOBL_ADDR:
		case PROC_ADDR:
			return reg_pointer;
		case CONST:
		case DCONST:
			return reg_any;
		default: assert(FALSE);
	}
	/* NOTREACHED */
}



STATIC short item_size(item)
	item_p item;
{
	/* Determine the size of the item (in bytes) */

	switch(item->it_type) {
		case LOCALVAR:
			return regv_size(item->i_t.it_off);
			/* use size mentioned in reg. message for local */
		case LOCAL_ADDR:
		case GLOBL_ADDR:
		case PROC_ADDR:
			return ps; /* pointer size */
		case CONST:
			return ws; /* word size */
		case DCONST:
			return 2 * ws; /* 2 * word size */
		default: assert(FALSE);
	}
	/* NOTREACHED */
}



STATIC init_item(a,b)
	item_p a,b;
{
	a->it_type = b->it_type;
	switch(a->it_type) {
		case GLOBL_ADDR:
			a->i_t.it_obj = b->i_t.it_obj;
			break;
		case PROC_ADDR:
			a->i_t.it_proc = b->i_t.it_proc;
			break;
		default:
			a->i_t.it_off = b->i_t.it_off;
	}
	a->it_usage = Lempty_set();
	a->it_regtype = reg_type(b);
	a->it_size = item_size(b);
	a->it_desirable = b->it_desirable;
}



STATIC void add_item(item,t,items)
	item_p item;
	time_p t;
	item_p items[];
{
	/* See if there was already a list element for item. In any
	 * case record the fact that item is used at 't'.
	 */

	register item_p x, *q;

	q = &items[item->it_type]; /* each type has its own list */
	for (x = *q; x != (item_p) 0; x = *q) {
		if (same_item(x,item)) {
			/* found */
			if (!item->it_desirable) {
				x->it_desirable = FALSE;
			}
			Ladd(t,&x->it_usage);
			return; /* done */
		}
		if (lt_item(item,x)) break;
		q = &x->it_next;
	}
	/* not found, allocate new item; q points to it_next field of
	 * the item after which the new item should be put.
	 */
	x = newitem();
	x->it_next = *q;
	*q = x;
	init_item(x,item);
	Ladd(t,&x->it_usage);
}



STATIC add_usage(l,b,items)
	line_p l;
	bblock_p b;
	item_p items[];
{
	/* An item is used at line l. Add it to the list of items.
	 * A local variable is only considered to be an item, if
	 * there is a register message for it; else its address
	 * is also considered to be an item.
	 */

	struct item thisitem;

	fill_item(&thisitem,l); /* fill in some fields */
	if (!desirable(l)) {
		thisitem.it_desirable = FALSE; /* don't put item in reg. */
	}
	if (thisitem.it_type == LOCALVAR && !is_regvar(thisitem.i_t.it_off)) {
		/* Use address of local instead of local itself */
		thisitem.it_type = LOCAL_ADDR;
		thisitem.it_regtype = reg_pointer;
	}
	add_item(&thisitem,cons_time(l,b),items);
}



build_itemlist(p,items,nrinstr_out)
	proc_p p;
	item_p items[];
	int    *nrinstr_out;
{
	/* Make a list of all items used in procedure p.
	 * An item is anything that can be put in a register,
	 * such as a local variable, a constant etc.
	 * As a side effect, determine the number of instructions of p.
	 */

	register line_p l;
	register bblock_p b;
	register cnt= 0;

	clean_tab(items);
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		for (l = b->b_start; l != (line_p) 0; l = l->l_next) {
			if (is_item(l)) {
				add_usage(l,b,items);
			}
			cnt++;
		}
	}
	*nrinstr_out = cnt;
}
