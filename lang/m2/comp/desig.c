/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* D E S I G N A T O R   E V A L U A T I O N */

/* $Id$ */

/*	Code generation for designators.
   	This file contains some routines that generate code common to address
	as well as value computations, and leave a description in a "desig"
	structure.  It also contains routines to load an address, load a value
	or perform a store.
*/

#include	"parameters.h"
#include	"debug.h"

#include	<stdlib.h>
#include	<assert.h>
#include	"em_arith.h"
#include	"em_label.h"
#include	"em_code.h"
#include	"alloc.h"

#include	"type.h"
#include	"LLlex.h"
#include	"def.h"
#include	"scope.h"
#include	"desig.h"
#include	"node.h"
#include	"warning.h"
#include	"error.h"
#include	"code.h"
#include	"tmpvar.h"
#include	"walk.h"

extern int	proclevel;
extern char	options[];

static int WordOrDouble(struct desig *ds, arith size)
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

void LOL(arith offset, arith size)
{
	if (size == word_size) {
		C_lol(offset);
	}
	else if (size == dword_size) {
		C_ldl(offset);
	}
	else {
		C_lal(offset);
		C_loi(size);
	}
}

void STL(arith offset, arith size)
{
	if (size == word_size) {
		C_stl(offset);
	}
	else if (size == dword_size) {
		C_sdl(offset);
	}
	else {
		C_lal(offset);
		C_sti(size);
	}
}

int DoLoad(register struct desig *ds, arith size)
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

int DoStore(register struct desig *ds, arith size)
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

	/*	Return 1 if the type indicated by tp has a size that is a
		multiple of the word_size and is also word_aligned
	*/
#define word_multiple(tp) \
	( (int)(tp->tp_size) % (int)word_size == 0 && \
	  tp->tp_align >= word_align)

	/*	Return 1 if the type indicated by tp has a size that is a proper
		dividor of the word_size, and has alignment >= size or
		alignment >= word_align
	*/
#define word_dividor(tp) \
	(	tp->tp_size < word_size &&  \
		(int)word_size % (int)(tp->tp_size) == 0 && \
		(tp->tp_align >= word_align || \
		 tp->tp_align >= (int)(tp->tp_size)))

#define USE_LOI_STI	0
#define USE_LOS_STS	1
#define USE_LOAD_STORE	2
#define USE_BLM		3	/* like USE_LOI_STI, but more restricted:
				   multiple of word_size only
				*/

static int suitable_move(register struct type *tp)
{
	/*	Find out how to load or store the value indicated by "ds".
		There are four ways:
		- suitable for BLM/LOI/STI
		- suitable for LOI/STI
		- suitable for LOS/STS/BLS
		- suitable for calls to load/store/blockmove
	*/

	if (! word_multiple(tp)) {
		if (word_dividor(tp)) return USE_LOI_STI;
		return USE_LOAD_STORE;
	}
	if (! fit(tp->tp_size, (int) word_size)) return USE_LOS_STS;
	return USE_BLM;
}

void CodeValue(register struct desig *ds, register struct type *tp)
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
		switch (suitable_move(tp)) {
		case USE_BLM:
		case USE_LOI_STI:
#ifndef SQUEEZE
			CodeAddress(ds);
			C_loi(tp->tp_size);
			break;
#endif
		case USE_LOS_STS:
			CodeAddress(ds);
			CodeConst(tp->tp_size, (int)pointer_size);
			C_los(pointer_size);
			break;
		case USE_LOAD_STORE:
			sz = WA(tp->tp_size);
			if (ds->dsg_kind != DSG_PFIXED) {
				arith tmp = NewPtr();

				CodeAddress(ds);
				STL(tmp, pointer_size);
				CodeConst(-sz, (int) pointer_size);
				C_ass(pointer_size);
				LOL(tmp, pointer_size);
				FreePtr(tmp);
			}
			else  {
				CodeConst(-sz, (int) pointer_size);
				C_ass(pointer_size);
				CodeAddress(ds);
			}
			CodeConst(tp->tp_size, (int) pointer_size);
			CAL("load", (int)pointer_size + (int)pointer_size);
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

void ChkForFOR(register struct node *nd)
{
	/*	Check for an assignment to a FOR-loop control variable
	*/
	if (nd->nd_class == Def) {
		register struct def *df = nd->nd_def;

		if (df->df_flags & D_FORLOOP) {
			node_warning(nd,
				     W_ORDINARY,
				     "assignment to FOR-loop control variable");
			df->df_flags &= ~D_FORLOOP;
					/* only procude warning once */
		}
	}
}

void CodeStore(register struct desig *ds, register struct type *tp)
{
	/*	Generate code to store the value on the stack in the designator
		described in "ds"
	*/

	switch(ds->dsg_kind) {
	case DSG_FIXED:
		if (DoStore(ds, tp->tp_size)) break;
		/* Fall through */
	case DSG_PLOADED:
	case DSG_PFIXED:
		CodeAddress(ds);
		switch (suitable_move(tp)) {
		case USE_BLM:
		case USE_LOI_STI:
#ifndef SQUEEZE
			C_sti(tp->tp_size);
			break;
#endif
		case USE_LOS_STS:
			CodeConst(tp->tp_size, (int) pointer_size);
			C_sts(pointer_size);
			break;
		case USE_LOAD_STORE:
			CodeConst(tp->tp_size, (int) pointer_size);
			C_cal("store");
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

void CodeCopy(register struct desig *lhs, register struct desig *rhs, arith sz, arith *psize)
{
	/*	Do part of a copy, which is assumed to be "reasonable",
		so that it can be done with LOI/STI or BLM.
	*/
	struct desig l, r;

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

struct desig null_desig;

void CodeMove(register struct desig *rhs, register struct node *left, struct type *rtp)
{
	/*	Generate code for an assignment. Testing of type
		compatibility and the like is already done.
		Go through some (considerable) trouble to see if a BLM can be
		generated.
	*/
	struct desig lhs;
	register struct type *tp = left->nd_type;
	int loadedflag = 0;

	lhs = null_desig;
	ChkForFOR(left);
	switch(rhs->dsg_kind) {
	case DSG_LOADED:
		CodeDesig(left, &lhs);
		if (rtp->tp_fund == T_STRING) {
			/* size of a string literal fits in an
			   int of size word_size
			*/
			CodeAddress(&lhs);
			C_loc(rtp->tp_size);
			C_loc(tp->tp_size);
			CAL("StringAssign", (int)pointer_size + (int)pointer_size + (int)dword_size);
			break;
		}
		CodeStore(&lhs, tp);
		break;
	case DSG_FIXED:
		CodeDesig(left, &lhs);
		if (lhs.dsg_kind == DSG_FIXED &&
		    fit(tp->tp_size, (int) word_size) &&
		    (int) (lhs.dsg_offset) % word_align ==
		    (int) (rhs->dsg_offset) % word_align) {
			register int sz = 1;
			arith size = tp->tp_size;

			while (size && sz < word_align) {
				/*	First copy up to word-aligned
					boundaries
				*/
				if (!((int)(lhs.dsg_offset)%(sz+sz))) {
					sz += sz;
				}
				else	CodeCopy(&lhs, rhs, (arith) sz, &size);
			}
			/*	Now copy the bulk
			*/
			sz = (int) size % (int) word_size;
			size -= sz;
			CodeCopy(&lhs, rhs, size, &size);
			size = sz;
			sz = word_size;
			while (size) {
				/*	And then copy remaining parts
				*/
				sz >>= 1;
				if (size >= sz) {
					CodeCopy(&lhs, rhs, (arith) sz, &size);
				}
			}
			break;
		}
		CodeAddress(&lhs);
		loadedflag = 1;
		/* Fall through */
	case DSG_PLOADED:
	case DSG_PFIXED:
		assert(! loadedflag || rhs->dsg_kind == DSG_FIXED);
		CodeAddress(rhs);
		if (loadedflag) {
			C_exg(pointer_size);
		}
		else {
			CodeDesig(left, &lhs);
			CodeAddress(&lhs);
		}
		switch (suitable_move(tp)) {
		case USE_BLM:
#ifndef SQUEEZE
			C_blm(tp->tp_size);
			break;
#endif
		case USE_LOS_STS:
			CodeConst(tp->tp_size, (int) pointer_size);
			C_bls(pointer_size);
			break;
		case USE_LOAD_STORE:
		case USE_LOI_STI:
			CodeConst(tp->tp_size, (int) pointer_size);
			CAL("blockmove", 3 * (int)pointer_size);
			break;
		}
		break;
	default:
		crash("CodeMove");
	}
}

void CodeAddress(register struct desig *ds)
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

void CodeFieldDesig(register struct def *df, register struct desig *ds)
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
		wds->w_flags |= df->df_flags;
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

void CodeVarDesig(register struct def *df, register struct desig *ds)
{
	/*	Generate code for a variable represented by a "def" structure.
		Of course, there are numerous cases: the variable is local,
		it is a value parameter, it is a var parameter, it is one of
		those of an enclosing procedure, or it is global.
	*/
	register struct scope *sc = df->df_scope;
	int difflevel;

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

	if ((difflevel = proclevel - sc->sc_level) != 0) {
		/* the variable is local to a statically enclosing procedure.
		*/
		assert(difflevel > 0);

		df->df_flags |= D_NOREG;
		if (df->df_flags & (D_VARPAR|D_VALPAR)) {
			/* value or var parameter
			*/
			C_lxa((arith) difflevel);
			if ((df->df_flags & D_VARPAR) ||
			    IsConformantArray(df->df_type)) {
				/* var parameter or conformant array.
				   The address is passed.
				*/
				C_adp(df->var_off);
				C_loi(pointer_size);
				ds->dsg_offset = 0;
				ds->dsg_kind = DSG_PLOADED;
				return;
			}
		}
		else	C_lxl((arith) difflevel);
		ds->dsg_kind = DSG_PLOADED;
		ds->dsg_offset = df->var_off;
		return;
	}

	/* Now, finally, we have a local variable or a local parameter
	*/
	if ((df->df_flags & D_VARPAR) ||
	    IsConformantArray(df->df_type)) {
		/* a var parameter; address directly accessible.
		*/
		ds->dsg_kind = DSG_PFIXED;
	}
	else	ds->dsg_kind = DSG_FIXED;
	ds->dsg_offset = df->var_off;
	ds->dsg_def = df;
}

void CodeDesig(register struct node *nd, register struct desig *ds)
{
	/*	Generate code for a designator. Use divide and conquer
		principle
	*/
	register struct def *df;

	switch(nd->nd_class) {	/* Divide */
	case Def:
		df = nd->nd_def;
		if (nd->nd_NEXT) CodeDesig(nd->nd_NEXT, ds);

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
		assert(nd->nd_symb == '[' || nd->nd_symb == ',');

		CodeDesig(nd->nd_LEFT, ds);
		CodeAddress(ds);
		CodePExpr(nd->nd_RIGHT);
		nd = nd->nd_LEFT;

		/* Now load address of descriptor
		*/
		if (IsConformantArray(nd->nd_type)) {
			arith off;
			assert(nd->nd_class == Def);

			df = nd->nd_def;
			off = df->var_off + pointer_size;
			if (proclevel > df->df_scope->sc_level) {
			    C_lxa((arith) (proclevel - df->df_scope->sc_level));
			    C_adp(off);
			}
			else	C_lal(off);
		}
		else	{
			C_loc(nd->nd_type->arr_low);
			C_sbu(int_size);
			c_lae_dlb(nd->nd_type->arr_descr);
		}
		if (options['A']) {
			C_cal("rcka");
		}
		ds->dsg_kind = DSG_INDEXED;
		break;

	case Arrow:
		assert(nd->nd_symb == '^');

		nd = nd->nd_RIGHT;
		CodeDesig(nd, ds);
		switch(ds->dsg_kind) {
		case DSG_LOADED:
			ds->dsg_kind = DSG_PLOADED;
			break;

		case DSG_INDEXED:
		case DSG_PLOADED:
		case DSG_PFIXED:
			CodeValue(ds, nd->nd_type);
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
