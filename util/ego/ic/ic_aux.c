/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N T E R M E D I A T E   C O D E
 *
 *  I C _ A U X . C
 */



#include <em_pseu.h>
#include <em_spec.h>
#include <em_mnem.h>
#include "../share/types.h"
#include "../share/global.h"
#include "../share/debug.h"
#include "../share/def.h"
#include "../share/aux.h"
#include "ic.h"
#include "ic_io.h"
#include "ic_lookup.h"
#include "../share/alloc.h"
#include "ic_aux.h"



/* opr_size */

offset opr_size(instr)
	short instr;
{
	switch(instr) {
		case op_loe:
		case op_ste:
		case op_ine:
		case op_dee:
		case op_zre:
			return (offset) ws;
		case op_lde:
		case op_sde:
			return (offset) 2*ws;
		case op_lae:
		case op_fil:
		case op_gto:
			return (offset) UNKNOWN_SIZE;
		default:
			error("illegal operand of opr_size: %d", instr);
	}
	/* NOTREACHED */
}



/* dblockdef */

STATIC offset argsize(arg)
	arg_p arg;
{
	/* Compute the size (in bytes) that the given initializer
	 * will occupy.
	 */

	offset s;
	argb_p argb;

	switch(arg->a_type) {
		case ARGOFF:
			/* See if value fits in a short */
			if ((short) arg->a_a.a_offset == arg->a_a.a_offset) {
				return ws;
			} else {
				return 2*ws;
			}
		case ARGINSTRLAB:
		case ARGOBJECT:
		case ARGPROC:
			return ps;  /* pointer size */
		case ARGSTRING:
			/* strings are partitioned into pieces */
			s = 0;
			for (argb = &arg->a_a.a_string; argb != (argb_p) 0;
			   argb = argb->ab_next) {
				s += argb->ab_index;
			}
			return s;
		case ARGICN:
		case ARGUCN:
		case ARGFCN:
			return arg->a_a.a_con.ac_length;
		default:
			assert(FALSE);
		}
		/* NOTREACHED */
}


STATIC offset blocksize(pseudo,args)
	byte  pseudo;
	arg_p args;
{
	/* Determine the number of bytes of a datablock */

	arg_p	arg;
	offset	sum;

	switch(pseudo) {
	   case DHOL:
	   case DBSS:
		if (args->a_type != ARGOFF) {
			error("offset expected");
		}
		return args->a_a.a_offset;
	   case DCON:
	   case DROM:
		sum = 0;
		for (arg = args; arg != (arg_p) 0; arg = arg->a_next) {
			/* Add the sizes of all initializers */
			sum += argsize(arg);
		}
		return sum;
	   default:
		assert(FALSE);
	}
	/* NOTREACHED */
}


STATIC arg_p copy_arg(arg)
	arg_p arg;
{
	/* Copy one argument */

	arg_p new;

	assert(arg->a_type == ARGOFF);
	new = newarg(ARGOFF);
	new->a_a.a_offset = arg->a_a.a_offset;
	return new;
}



STATIC arg_p copy_rom(args)
	arg_p args;
{
	/* Make a copy of the values of a rom,
	 * provided that the rom contains only integer values,
	 */

	arg_p arg, arg2, argh;

	for (arg = args; arg != (arg_p) 0; arg = arg->a_next) {
		if (arg->a_type != ARGOFF) {
			return (arg_p) 0;
		}
	}
	/* Now make the copy */
	arg2 = argh = copy_arg(args);
	for (arg = args->a_next; arg != (arg_p) 0; arg = arg->a_next) {
		arg2->a_next = copy_arg(arg);
		arg2 = arg2->a_next;
	}
	return argh;
}



dblockdef(db,n,lnp)
	dblock_p db;
	int	 n;
	line_p	 lnp;
{
	/* Process a data block defining occurrence */

	byte m;

	switch(n) {
		case ps_hol:
			m = DHOL;
			break;
		case ps_bss:
			m = DBSS;
			break;
		case ps_con:
			m = DCON;
			break;
		case ps_rom:
			m = DROM;
			break;
		default:
			assert(FALSE);
	}
	db->d_pseudo = m;
	db->d_size = blocksize(m, ARG(lnp));
	if (m == DROM) {
		/* We keep the values of a rom block in the data block
		 * table if the values consist of integers only.
		 */
		db->d_values = copy_rom(ARG(lnp));
	}
}



/* combine */

combine(db,l1,l2,pseu)
	dblock_p db;
	line_p   l1,l2;
	byte pseu;
{
	/* Combine two successive ROMs/CONs (without a data label
	 * in between into a single ROM. E.g.:
	 *    xyz
	 *     rom 3,6,9,12
	 *     rom 7,0,2
	 * is changed into:
	 *    xyz
	 *     rom 3,6,9,12,7,0,2
	 */

	arg_p v;

	db->d_size += blocksize(pseu,ARG(l2));
	/* db is the data block that was already assigned to the
	 * first rom/con. The second one is not assigned a new
	 * data block of course, as the two are combined into
	 * one instruction.
	 */
	if (pseu == DROM && db->d_values != (arg_p) 0) {
		/* The values contained in a ROM are only copied
		 * to the data block if they may be useful to us
		 * (e.g. they certainly may not be strings). In our
		 * case it means that both ROMs must have useful
		 * arguments.
		 */
		for (v = db->d_values; v->a_next != (arg_p) 0; v = v->a_next);
		/* The first rom contained useful arguments. v now points to
		 * its last argument. Append the arguments of the second
		 * rom to this list. If the second rom has arguments that are
		 * not useful, throw away the entire list (we want to copy
		 * everything or nothing).
		 */
		if ((v->a_next = copy_rom(ARG(l2))) == (arg_p) 0) {
			oldargs(db->d_values);
			db->d_values = (arg_p) 0;
		}
	}
	for (v = ARG(l1); v->a_next != (arg_p) 0; v = v->a_next);
	/* combine the arguments of both instructions. */
	v->a_next = ARG(l2);
	ARG(l2) = (arg_p) 0;
}



/* arglist */

STATIC arg_string(length,abp)
	offset  length;
	register argb_p abp;
{

	while (length--) {
		if (abp->ab_index == NARGBYTES)
			abp = abp->ab_next = newargb();
		abp->ab_contents[abp->ab_index++] = readchar();
	}
}


line_p arglist(n)
	int n;
{
	line_p	lnp;
	register arg_p ap,*app;
	bool moretocome;
	offset length;


	/*
	 * creates an arglist with n elements
	 * if n == 0 the arglist is variable and terminated by sp_cend
	 */

	lnp = newline(OPLIST);
	app = &ARG(lnp);
	moretocome = TRUE;
	do {
		switch(table2()) {
		default:
			error("unknown byte in arglist");
		case CSTX1:
			tabval2 = (offset) tabval;
		case CSTX2:
			*app = ap = newarg(ARGOFF);
			ap->a_a.a_offset = tabval2;
			app = &ap->a_next;
			break;
		case ILBX:
			*app = ap = newarg(ARGINSTRLAB);
			ap->a_a.a_instrlab = instr_lab((short) tabval);
			app = &ap->a_next;
			break;
		case DLBX:
			*app = ap = newarg(ARGOBJECT);
			ap->a_a.a_obj = object(string,(offset) 0, (offset) 0);
			/* The size of the object is unknown */
			app = &ap->a_next;
			break;
		case sp_pnam:
			*app = ap = newarg(ARGPROC);
			ap->a_a.a_proc = proclookup(string,OCCURRING);
			app = &ap->a_next;
			break;
		case VALX1:
			tabval2 = (offset) tabval;
		case VALX2:
			*app = ap = newarg(ARGOBJECT);
			ap->a_a.a_obj = object(string, tabval2, (offset) 0);
			app = &ap->a_next;
			break;
		case sp_scon:
			*app = ap = newarg(ARGSTRING);
			length = get_off();
			arg_string(length,&ap->a_a.a_string);
			app = &ap->a_next;
			break;
		case sp_icon:
			*app = ap = newarg(ARGICN);
			goto casecon;
		case sp_ucon:
			*app = ap = newarg(ARGUCN);
			goto casecon;
		case sp_fcon:
			*app = ap = newarg(ARGFCN);
		casecon:
			length = get_int();
			ap->a_a.a_con.ac_length = (short) length;
			arg_string(get_off(),&ap->a_a.a_con.ac_con);
			app = &ap->a_next;
			break;
		case sp_cend:
			moretocome = FALSE;
		}
		if (n && (--n) == 0)
			moretocome = FALSE;
	} while (moretocome);
	return(lnp);
}



/* is_datalabel */

bool is_datalabel(l)
	line_p l;
{
	VL(l);
	return (l->l_instr == (byte) ps_sym);
}



/* block_of_lab */

dblock_p block_of_lab(ident)
	char *ident;
{
	dblock_p dbl;

	/* Find the datablock with the given name.
	 * Used for defining occurrences.
	 */

	dbl = symlookup(ident,DEFINING);
	VD(dbl);
	if (dbl->d_pseudo != DUNKNOWN) {
		error("identifier %s redeclared", ident);
	}
	return dbl;
}



/* object */

STATIC obj_p make_object(dbl,off,size)
	dblock_p dbl;
	offset   off;
	offset   size;
{
	/* Allocate an obj struct with the given attributes
	 * (if it did not exist already).
	 * Return a pointer to the found or newly created object struct.
	 */

	obj_p obj, prev, new;

	/* See if the object was already present in the object list
	 *  of the given datablock. If it is not yet present, find
	 * the right place to insert the new object. Note that
	 * the objects are sorted by offset.
	 */
	prev = (obj_p) 0;
	for (obj = dbl->d_objlist; obj != (obj_p) 0; obj = obj->o_next) {
		if (obj->o_off >= off) {
			break;
		}
		prev = obj;
	}
	/* Note that the data block may contain several objects
	 * with the required offset; we also want the size to
	 * be the right one.
	 */
	while (obj != (obj_p) 0 && obj->o_off == off) {
		if (obj->o_size == UNKNOWN_SIZE) {
			obj->o_size = size;
			return obj;
		} else {
			if (size == UNKNOWN_SIZE || obj->o_size == size) {
				return obj;
				/* This is the right one */
			} else {
				prev = obj;
				obj = obj->o_next;
			}
		}
	}
	/* Allocate a new object */
	new = newobject();
	new->o_id     = ++lastoid;	/* create a unique object id */
	new->o_off    = off;
	new->o_size   = size;
	new->o_dblock = dbl;
	/* Insert the new object */
	if (prev == (obj_p) 0) {
		dbl->d_objlist = new;
	} else {
		prev->o_next = new;
	}
	new->o_next = obj;
	return new;
}



obj_p object(ident,off,size)
	char *ident;
	offset off;
	offset size;
{
	dblock_p dbl;

	/* Create an object struct (if it did not yet exist)
	 * for the object with the given size and offset
	 * within the datablock of the given name.
	 */

	dbl = (ident == (char *) 0 ? hol0_db : symlookup(ident, OCCURRING));
	VD(dbl);
	return(make_object(dbl,off,size));
}
