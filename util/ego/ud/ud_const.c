/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* C O N S T A N T   P R O P A G A T I O N */

#include <em_mnem.h>
#include <em_pseu.h>
#include <em_spec.h>
#include "../share/types.h"
#include "ud.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/alloc.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "../share/def.h"
#include "../share/aux.h"
#include "../share/locals.h"
#include "ud_defs.h"
#include "ud_const.h"
#include "ud_aux.h"


#define IS_REG(v)	(locals[TO_LOCAL(v)]->lc_flags & LCF_REG)
#define CALLS_UNKNOWN(p) (p->p_flags1 & (byte) PF_CALUNKNOWN)


bool is_use(l)
	line_p l;
{
	/* See if 'l' is a use of a variable */

	switch(INSTR(l)) {
		 case op_lde:
		 case op_ldl:
		 case op_loe:
		 case op_lol:
			return TRUE;
		default:
			return FALSE;
	}
	/* NOTREACHED  */
}




bool value_known(def,val_out)
	line_p def;
	offset *val_out;
{
	/* See if the value stored by definition 'def'
	 * is known statically (i.e. is a constant).
	 */

	short sz1, sz2;
	offset v;
	line_p l;

	sz1 = ws;
	switch(INSTR(def)) {
		case op_inl:
		case op_ine:
		case op_del:
		case op_dee:
			return FALSE;
		case op_zrl:
		case op_zre:
			v = (offset) 0;
			break;
		case op_sdl:
		case op_sde:
			sz1 += ws;
			/* fall through ... */
		case op_stl:
		case op_ste:
			l = PREV(def);
			if (l == (line_p) 0) return FALSE;
			sz2 = ws;
			switch(INSTR(l)) {
				case op_zer:
					if (SHORT(l) >= sz1) {
						v = (offset) 0;
						break;
					}
					return FALSE;
				case op_ldc:
					sz2 += ws;
					/* fall through ...*/
				case op_loc:
					if (sz1 == sz2) {
						v = off_set(l);
						break;
					}
					/* fall through ... */
				default:
					return FALSE;
			}
			break;
		default:
			assert(FALSE);
	}
	*val_out = v;
	return TRUE;
}




bool affected(use,v,l)
	line_p use,l;
	short  v;
{
	/* See if the variable referenced by 'use' may be
	 * changed by instruction l, which is either a cal, cai or
	 * an indirect assignment.
	 */

	if (INSTR(l) == op_cal &&
	    TYPE(use) == OPOBJECT &&
	    BODY_KNOWN(PROC(l)) &&
	    !CALLS_UNKNOWN(PROC(l)) &&
	    !CHANGE_INDIR(PROC(l))) {
		return Cis_elem(OBJ(use)->o_id,PROC(l)->p_change->c_ext);
	}
	return TYPE(use) == OPOBJECT || !IS_REG(v);
}




STATIC search_backwards(use,v,found,def)
	line_p use, *def;
	short v;
	bool *found;
{
	/* Search backwards in the current basic block,
	 * starting at 'use', trying to find a definition
	 * of the variable referenced by 'use', whose variable
	 * number is v. If the definition found is an
	 * implicit one, return 0 as def.
	 */

	register line_p l;

	for (l = PREV(use); l != (line_p) 0; l = PREV(l)) {
		if (does_expl_def(l) && same_var(use,l)) {
			*found = TRUE;
			*def = l;
			return;
		}
		if (does_impl_def(l) && affected(use,v,l)) {
			*found = TRUE;
			*def = (line_p) 0;
			return;
		}
	}
	*found = FALSE;
}




STATIC short outer_def(vdefs,in)
	cset vdefs, in;
{
	/* See if there is a unique definition of variable
	 * v reaching the beginning of block b.
	 * 'vdefs' is vardefs[v], 'in' is IN(b).
	 */

	short n,defnr = 0;
	Cindex i;

	for (i = Cfirst(vdefs); i != (Cindex) 0; i = Cnext(i,vdefs)) {
		n = Celem(i);
		if (Cis_elem(EXPL_TO_DEFNR(n),in)) {
			if (defnr != 0) return  0;
			/* If there was already a def., there's no unique one */
			defnr = n;
		}
	}
	return defnr;
}




line_p unique_def(use,b,defnr_out)
	line_p use;
	bblock_p b;
	short *defnr_out;
{
	/* See if there is one unique explicit definition
	 * of the variable used by 'use', that reaches 'use'.
	 */

	short v;
	bool found;
	line_p def =  (line_p) 0;

	*defnr_out = 0;
	var_nr(use,&v,&found);
	if (found) {
		/* We do maintain ud-info for this variable.
		 * See if there is a previous explicit definition
		 * in the current basic block.
		 */
		search_backwards(use,v,&found,&def);
		if (!found && !Cis_elem(IMPLICIT_DEF(v),IN(b))) {
			/* See if there is a unique explicit definition
			 * outside the current block, reaching the
			 * beginning of the current block.
			 */
			*defnr_out = outer_def(vardefs[v],IN(b));
			def = (*defnr_out == 0 ? (line_p) 0 : defs[*defnr_out]);
		}
	}
	return def;
}



fold_const(l,b,val)
	line_p l;
	bblock_p b;
	offset val;
{
	/* Perform the substitutions required for constant folding */

	line_p n;

	n = int_line(val);
	switch(INSTR(l)) {
		case op_lol:
		case op_loe:
			n->l_instr = op_loc;
			break;
		case op_ldl:
		case op_lde:
			n->l_instr = op_ldc;
			break;
		default:
			assert (FALSE);
	}
	repl_line(l,n,b);
}
