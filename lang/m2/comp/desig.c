/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* D E S I G N A T O R   E V A L U A T I O N */

/* $Header$ */

/*	Code generation for designators.
   	This file contains some routines that generate code common to address
	as well as value computations, and leave a description in a "desig"
	structure.  It also contains routines to load an address, load a value
	or perform a store.
*/

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<assert.h>
#include	<alloc.h>

#include	"type.h"
#include	"LLlex.h"
#include	"def.h"
#include	"scope.h"
#include	"desig.h"
#include	"node.h"
#include	"warning.h"
#include	"walk.h"

extern int	proclevel;
extern arith	NewPtr();
extern char	options[];

int
WordOrDouble(ds, size)
	t_desig *ds;
	arith size;
{
	/*	Check if designator is suitable for word or double-word
		operation
	*/
	if ((int) (ds->dsg_offset) % word_align == 0) {
		if (size == word_size) return 1;
		if (size == dword_size) return 2;
	}
	return 0;
}

int
DoLoad(ds, size)
	register t_desig *ds;
	arith size;
{
	/*	Try to load designator with word or double-word operation.
		Return 0 if not done
	*/
	switch (WordOrDouble(ds, size)) {
	default:
		return 0;
	case 1:
		if (ds->dsg_name) {
			C_loe_dnam(ds->dsg_name, ds->dsg_offset);
		}
		else	C_lol(ds->dsg_offset);
		break;
	case 2:
		if (ds->dsg_name) {
			C_lde_dnam(ds->dsg_name, ds->dsg_offset);
		}
		else	C_ldl(ds->dsg_offset);
		break;
	}
	return 1;
}

int
DoStore(ds, size)
	register t_desig *ds;
	arith size;
{
	/*	Try to store designator with word or double-word operation.
		Return 0 if not done
	*/
	switch (WordOrDouble(ds, size)) {
	default:
		return 0;
	case 1:
		if (ds->dsg_name) {
			C_ste_dnam(ds->dsg_name, ds->dsg_offset);
		}
		else	C_stl(ds->dsg_offset);
		break;
	case 2:
		if (ds->dsg_name) {
			C_sde_dnam(ds->dsg_name, ds->dsg_offset);
		}
		else	C_sdl(ds->dsg_offset);
		break;
	}
	return 1;
}

int
word_multiple(tp)
	register t_type *tp;
{
	/*	Return 1 if the type indicated by tp has a size that is a
		multiple of the word_size and is also word_aligned
	*/
	return  (int)(tp->tp_size) % (int)word_size == 0 &&
		tp->tp_align >= word_align;
}

int
word_dividor(tp)
	register t_type *tp;
{
	/*	Return 1 if the type indicated by tp has a size that is a proper
		dividor of the word_size, and has alignment >= size or
		alignment >= word_align
	*/
	return	tp->tp_size < word_size && 
		(int)word_size % (int)(tp->tp_size) == 0 &&
		(tp->tp_align >= word_align ||
		 tp->tp_align >= (int)(tp->tp_size));
}

#define USE_LOI_STI	0
#define USE_LOS_STS	1
#define USE_LOAD_STORE	2
#define USE_BLM		3	/* like USE_LOI_STI, but more restricted:
				   multiple of word_size only
				*/

STATIC int
type_to_stack(tp)
	register t_type *tp;
{
	/*	Find out how to load or store the value indicated by "ds".
		There are three ways:
		- with LOI/STI
		- with LOS/STS
		- with calls to _load/_store
	*/

	if (! word_multiple(tp)) {
		if (word_dividor(tp)) return USE_LOI_STI;
		return USE_LOAD_STORE;
	}
	if (! fit(tp->tp_size, (int) word_size)) return USE_LOS_STS;
	return USE_BLM;
}

CodeValue(ds, tp)
	register t_desig *ds;
	register t_type *tp;
{
	/*	Generate code to load the value of the designator described
		in "ds".
	*/
	arith sz;

	switch(ds->dsg_kind) {
	case DSG_LOADED:
		break;

	case DSG_FIXED:
		if (DoLoad(ds, tp->tp_size)) break;
		/* Fall through */
	case DSG_PLOADED:
	case DSG_PFIXED:
		switch (type_to_stack(tp)) {
		case USE_BLM:
		case USE_LOI_STI:
			CodeAddress(ds);
			C_loi(tp->tp_size);
			break;
		case USE_LOS_STS:
			CodeAddress(ds);
			CodeConst(tp->tp_size, (int)pointer_size);
			C_los(pointer_size);
			break;
		case USE_LOAD_STORE:
			sz = WA(tp->tp_size);
			if (ds->dsg_kind == DSG_PLOADED) {
				arith tmp = NewPtr();

				CodeAddress(ds);
				C_lal(tmp);
				C_sti(pointer_size);
				CodeConst(-sz, (int) pointer_size);
				C_ass(pointer_size);
				C_lal(tmp);
				C_loi(pointer_size);
				FreePtr(tmp);
			}
			else  {
				CodeConst(-sz, (int) pointer_size);
				C_ass(pointer_size);
			}
			CodeAddress(ds);
			CodeConst(tp->tp_size, (int) pointer_size);
			C_cal("_load");
			C_asp(pointer_size + pointer_size);
			break;
		}
		break;

	case DSG_INDEXED:
		C_lar(word_size);
		break;

	default:
		crash("(CodeValue)");
	}

	ds->dsg_kind = DSG_LOADED;
}

ChkForFOR(nd)
	t_node *nd;
{
	/*	Check for an assignment to a FOR-loop control variable
	*/
	if (nd->nd_class == Def) {
		register t_def *df = nd->nd_def;

		if (df->df_flags & D_FORLOOP) {
			node_warning(nd,
				     W_ORDINARY,
				     "assignment to FOR-loop control variable");
			df->df_flags &= ~D_FORLOOP;
					/* only procude warning once */
		}
	}
}

CodeStore(ds, tp)
	register t_desig *ds;
	register t_type *tp;
{
	/*	Generate code to store the value on the stack in the designator
		described in "ds"
	*/
	t_desig save;

	save = *ds;

	switch(ds->dsg_kind) {
	case DSG_FIXED:
		if (DoStore(ds, tp->tp_size)) break;
		/* Fall through */
	case DSG_PLOADED:
	case DSG_PFIXED:
		CodeAddress(&save);
		switch (type_to_stack(tp)) {
		case USE_BLM:
		case USE_LOI_STI:
			C_sti(tp->tp_size);
			break;
		case USE_LOS_STS:
			CodeConst(tp->tp_size, (int) pointer_size);
			C_sts(pointer_size);
			break;
		case USE_LOAD_STORE:
			CodeConst(tp->tp_size, (int) pointer_size);
			C_cal("_store");
			CodeConst(pointer_size + pointer_size + WA(tp->tp_size),
			  	(int) pointer_size);
			C_ass(pointer_size);
			break;
		}
		break;

	case DSG_INDEXED:
		C_sar(word_size);
		break;

	default:
		crash("(CodeStore)");
	}

	ds->dsg_kind = DSG_INIT;
}

CodeCopy(lhs, rhs, sz, psize)
	register t_desig *lhs, *rhs;
	arith sz, *psize;
{
	/*	Do part of a copy, which is assumed to be "reasonable",
		so that it can be done with LOI/STI or BLM.
	*/
	t_desig l, r;

	l = *lhs; r = *rhs;
	*psize -= sz;
	lhs->dsg_offset += sz;
	rhs->dsg_offset += sz;
	CodeAddress(&r);
	if (sz <= dword_size) {
		C_loi(sz);
		CodeAddress(&l);
		C_sti(sz);
	}
	else {
		CodeAddress(&l);
		C_blm(sz);
	}
}

CodeMove(rhs, left, rtp)
	register t_desig *rhs;
	register t_node *left;
	t_type *rtp;
{
	/*	Generate code for an assignment. Testing of type
		compatibility and the like is already done.
		Go through some (considerable) trouble to see if a BLM can be
		generated.
	*/
	register t_desig *lhs = new_desig();
	register t_type *tp = left->nd_type;

	ChkForFOR(left);
	switch(rhs->dsg_kind) {
	case DSG_LOADED:
		CodeDesig(left, lhs);
		if (rtp->tp_fund == T_STRING) {
			/* size of a string literal fits in an
			   int of size word_size
			*/
			CodeAddress(lhs);
			C_loc(rtp->tp_size);
			C_loc(tp->tp_size);
			C_cal("_StringAssign");
			C_asp(pointer_size + pointer_size + dword_size);
			break;
		}
		CodeStore(lhs, tp);
		break;
	case DSG_FIXED:
		if (lhs->dsg_kind == DSG_FIXED &&
		    fit(tp->tp_size, (int) word_size) &&
		    (int) (lhs->dsg_offset) % (int) word_size ==
		    (int) (rhs->dsg_offset) % (int) word_size) {
			register int sz;
			arith size = tp->tp_size;

			CodeDesig(left, lhs);
			while (size &&
			       (sz = ((int)(lhs->dsg_offset)%(int)word_size))) {
				/*	First copy up to word-aligned
					boundaries
				*/
				if (sz < 0) sz = -sz;	/* bloody '%' */
				while ((int) word_size % sz) sz--;
				CodeCopy(lhs, rhs, (arith) sz, &size);
			}
			if (size > 3*dword_size) {
				/*	Do a block move
				*/
				arith sz;

				sz = size - size % word_size;
				CodeCopy(lhs, rhs, sz, &size);
			}
			else for (sz = (int) dword_size;
				  sz; sz -= (int) word_size) {
				while (size >= sz) {
					/*	Then copy dwords, words.
						Depend on peephole optimizer
					*/
					CodeCopy(lhs, rhs, (arith) sz, &size);
				}
			}
			sz = word_size;
			while (size && --sz) {
				/*	And then copy remaining parts
				*/
				while ((int) word_size % sz) sz--;
				while (size >= sz) {
					CodeCopy(lhs, rhs, (arith) sz, &size);
				}
			}
			break;
		}
		/* Fall through */
	case DSG_PLOADED:
	case DSG_PFIXED:
		CodeAddress(rhs);
		CodeDesig(left, lhs);
		CodeAddress(lhs);
		switch (type_to_stack(tp)) {
		case USE_BLM:
			C_blm(tp->tp_size);
			break;
		case USE_LOS_STS:
			CodeConst(tp->tp_size, (int) pointer_size);
			C_bls(pointer_size);
			break;
		case USE_LOAD_STORE:
		case USE_LOI_STI:
			CodeConst(tp->tp_size, (int) pointer_size);
			C_cal("_blockmove");
			C_asp(3 * pointer_size);
			break;
		}
		break;
	default:
		crash("CodeMove");
	}
	free_desig(lhs);
}

CodeAddress(ds)
	register t_desig *ds;
{
	/*	Generate code to load the address of the designator described
	   	in "ds"
	*/

	switch(ds->dsg_kind) {
	case DSG_PLOADED:
		if (ds->dsg_offset) {
			C_adp(ds->dsg_offset);
		}
		break;

	case DSG_FIXED:
		if (ds->dsg_name) {
			C_lae_dnam(ds->dsg_name, ds->dsg_offset);
			break;
		}
		C_lal(ds->dsg_offset);
		if (ds->dsg_def) ds->dsg_def->df_flags |= D_NOREG;
		break;
		
	case DSG_PFIXED:
		if (! DoLoad(ds, pointer_size)) {
			assert(0);
		}
		break;

	case DSG_INDEXED:
		C_aar(word_size);
		break;

	default:
		crash("(CodeAddress)");
	}

	ds->dsg_offset = 0;
	ds->dsg_kind = DSG_PLOADED;
}

CodeFieldDesig(df, ds)
	register t_def *df;
	register t_desig *ds;
{
	/* Generate code for a field designator. Only the code common for
	   address as well as value computation is generated, and the
	   resulting information on where to find the designator is placed
	   in "ds". "df" indicates the definition of the field.
	*/

	if (ds->dsg_kind == DSG_INIT) {
		/* In a WITH statement. We must find the designator in the
		   WITH statement, and act as if the field is a selection
		   of this designator.
		   So, first find the right WITH statement, which is the
		   first one of the proper record type, which is
		   recognized by its scope indication.
		*/
		register struct withdesig *wds = WithDesigs;

		assert(wds != 0);

		while (wds->w_scope != df->df_scope) {
			wds = wds->w_next;
			assert(wds != 0);
		}

		/* Found it. Now, act like it was a selection.
		*/
		*ds = wds->w_desig;
		assert(ds->dsg_kind == DSG_PFIXED);
	}

	switch(ds->dsg_kind) {
	case DSG_PLOADED:
	case DSG_FIXED:
		ds->dsg_offset += df->fld_off;
		break;

	case DSG_PFIXED:
	case DSG_INDEXED:
		CodeAddress(ds);
		ds->dsg_kind = DSG_PLOADED;
		ds->dsg_offset = df->fld_off;
		break;

	default:
		crash("(CodeFieldDesig)");
	}
}

CodeVarDesig(df, ds)
	register t_def *df;
	register t_desig *ds;
{
	/*	Generate code for a variable represented by a "def" structure.
		Of course, there are numerous cases: the variable is local,
		it is a value parameter, it is a var parameter, it is one of
		those of an enclosing procedure, or it is global.
	*/
	register t_scope *sc = df->df_scope;

	/* Selections from a module are handled earlier, when identifying
	   the variable, so ...
	*/
	assert(ds->dsg_kind == DSG_INIT);

	if (df->df_flags & D_ADDRGIVEN) {
		/* the programmer specified an address in the declaration of
		   the variable. Generate code to push the address.
		*/
		CodeConst(df->var_off, (int) pointer_size);
		ds->dsg_kind = DSG_PLOADED;
		ds->dsg_offset = 0;
		return;
	}

	if (df->var_name) {
		/* this variable has been given a name, so it is global.
		   It is directly accessible.
		*/
		ds->dsg_name = df->var_name;
		ds->dsg_offset = 0;
		ds->dsg_kind = DSG_FIXED;
		return;
	}

	if (sc->sc_level != proclevel) {
		/* the variable is local to a statically enclosing procedure.
		*/
		assert(proclevel > sc->sc_level);

		df->df_flags |= D_NOREG;
		if (df->df_flags & (D_VARPAR|D_VALPAR)) {
			/* value or var parameter
			*/
			C_lxa((arith) (proclevel - sc->sc_level));
			if ((df->df_flags & D_VARPAR) ||
			    IsConformantArray(df->df_type)) {
				/* var parameter or conformant array.
				   For conformant array's, the address is
				   passed.
				*/
				C_adp(df->var_off);
				C_loi(pointer_size);
				ds->dsg_offset = 0;
				ds->dsg_kind = DSG_PLOADED;
				return;
			}
		}
		else	C_lxl((arith) (proclevel - sc->sc_level));
		ds->dsg_kind = DSG_PLOADED;
		ds->dsg_offset = df->var_off;
		return;
	}

	/* Now, finally, we have a local variable or a local parameter
	*/
	if ((df->df_flags & D_VARPAR) || IsConformantArray(df->df_type)) {
		/* a var parameter; address directly accessible.
		*/
		ds->dsg_kind = DSG_PFIXED;
	}
	else	ds->dsg_kind = DSG_FIXED;
	ds->dsg_offset = df->var_off;
	ds->dsg_def = df;
}

CodeDesig(nd, ds)
	register t_node *nd;
	register t_desig *ds;
{
	/*	Generate code for a designator. Use divide and conquer
		principle
	*/
	register t_def *df;

	switch(nd->nd_class) {	/* Divide */
	case Def:
		df = nd->nd_def;
		if (nd->nd_left) CodeDesig(nd->nd_left, ds);

		switch(df->df_kind) {
		case D_FIELD:
			CodeFieldDesig(df, ds);
			break;

		case D_VARIABLE:
			CodeVarDesig(df, ds);
			break;

		default:
			crash("(CodeDesig) Def");
		}
		break;

	case Arrsel:
		assert(nd->nd_symb == '[');

		CodeDesig(nd->nd_left, ds);
		CodeAddress(ds);
		CodePExpr(nd->nd_right);

		/* Now load address of descriptor
		*/
		if (IsConformantArray(nd->nd_left->nd_type)) {
			assert(nd->nd_left->nd_class == Def);

			df = nd->nd_left->nd_def;
			if (proclevel > df->df_scope->sc_level) {
			    C_lxa((arith) (proclevel - df->df_scope->sc_level));
			    C_adp(df->var_off + pointer_size);
			}
			else	C_lal(df->var_off + pointer_size);
		}
		else	{
			C_loc(nd->nd_left->nd_type->arr_low);
			C_sbu(int_size);
			c_lae_dlb(nd->nd_left->nd_type->arr_descr);
		}
		if (options['A']) {
			C_cal("rcka");
		}
		ds->dsg_kind = DSG_INDEXED;
		break;

	case Arrow:
		assert(nd->nd_symb == '^');

		CodeDesig(nd->nd_right, ds);
		switch(ds->dsg_kind) {
		case DSG_LOADED:
			ds->dsg_kind = DSG_PLOADED;
			break;

		case DSG_INDEXED:
		case DSG_PLOADED:
		case DSG_PFIXED:
			CodeValue(ds, nd->nd_right->nd_type);
			ds->dsg_kind = DSG_PLOADED;
			ds->dsg_offset = 0;
			break;

		case DSG_FIXED:
			ds->dsg_kind = DSG_PFIXED;
			break;

		default:
			crash("(CodeDesig) Uoper");
		}
		break;
		
	default:
		crash("(CodeDesig) class");
	}
}
