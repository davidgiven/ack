/* D E S I G N A T O R   E V A L U A T I O N */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

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

#include	"type.h"
#include	"def.h"
#include	"scope.h"
#include	"desig.h"
#include	"LLlex.h"
#include	"node.h"

extern int	proclevel;
struct desig	InitDesig = {DSG_INIT, 0, 0};

CodeValue(ds, size)
	register struct desig *ds;
	arith size;
{
	/*	Generate code to load the value of the designator described
		in "ds"
	*/

	switch(ds->dsg_kind) {
	case DSG_LOADED:
		break;

	case DSG_FIXED:
		if (size == word_size) {
			if (ds->dsg_name) {
				C_loe_dnam(ds->dsg_name, ds->dsg_offset);
			}
			else	C_lol(ds->dsg_offset);
			break;
		}

		if (size == dword_size) {
			if (ds->dsg_name) {
				C_lde_dnam(ds->dsg_name, ds->dsg_offset);
			}
			else	C_ldl(ds->dsg_offset);
			break;
		}
		/* Fall through */
	case DSG_PLOADED:
	case DSG_PFIXED:
		CodeAddress(ds);
		C_loi(size);
		break;

	case DSG_INDEXED:
		C_lar(word_size);
		break;

	default:
		crash("(CodeValue)");
	}

	ds->dsg_kind = DSG_LOADED;
}

CodeStore(ds, size)
	register struct desig *ds;
	arith size;
{
	/*	Generate code to store the value on the stack in the designator
		described in "ds"
	*/

	switch(ds->dsg_kind) {
	case DSG_FIXED:
		if (size == word_size) {
			if (ds->dsg_name) {
				C_ste_dnam(ds->dsg_name, ds->dsg_offset);
			}
			else	C_stl(ds->dsg_offset);
			break;
		}

		if (size == dword_size) {
			if (ds->dsg_name) {
				C_sde_dnam(ds->dsg_name, ds->dsg_offset);
			}
			else	C_sdl(ds->dsg_offset);
			break;
		}
		/* Fall through */
	case DSG_PLOADED:
	case DSG_PFIXED:
		CodeAddress(ds);
		C_sti(size);
		break;

	case DSG_INDEXED:
		C_sar(word_size);
		break;

	default:
		crash("(CodeStore)");
	}

	ds->dsg_kind = DSG_INIT;
}

CodeAddress(ds)
	register struct desig *ds;
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
		break;
		
	case DSG_PFIXED:
		ds->dsg_kind = DSG_FIXED;
		CodeValue(ds, pointer_size);
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
	register struct def *df;
	register struct desig *ds;
{
	/* Generate code for a field designator. Only the code common for
	   address as well as value computation is generated, and the
	   resulting information on where to find the designator is placed
	   in "ds". "df" indicates the definition of the field.
	*/

	register struct withdesig *wds;

	if (ds->dsg_kind == DSG_INIT) {
		/* In a WITH statement. We must find the designator in the
		   WITH statement, and act as if the field is a selection
		   of this designator.
		   So, first find the right WITH statement, which is the
		   first one of the proper record type.
		   Notice that the proper record type is recognized by its
		   scope indication.
		*/
		wds = WithDesigs;
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
	register struct def *df;
	register struct desig *ds;
{
	/*	Generate code for a variable represented by a "def" structure.
		Of course, there are numerous cases: the variable is local,
		it is a value parameter, it is a var parameter, it is one of
		those of an enclosing procedure, or it is global.
	*/
	register struct scope *sc = df->df_scope;

	/* Selections from a module are handled earlier, when identifying
	   the variable, so ...
	*/
	assert(ds->dsg_kind == DSG_INIT);

	df->df_flags |= D_USED;
	if (df->var_addrgiven) {
		/* the programmer specified an address in the declaration of
		   the variable. Generate code to push the address.
		*/
		CodeConst(df->var_off, pointer_size);
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
				/* var parameter
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
	ds->dsg_offset =df->var_off;
}

CodeDesig(nd, ds)
	register struct node *nd;
	register struct desig *ds;
{
	/*	Generate code for a designator. Use divide and conquer
		principle
	*/
	register struct def *df;

	switch(nd->nd_class) {	/* Divide */
	case Def:
		df = nd->nd_def;

		df->df_flags |= D_USED;
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

	case LinkDef:
		assert(nd->nd_symb == '.');

		CodeDesig(nd->nd_left, ds);
		CodeFieldDesig(nd->nd_def, ds);
		break;

	case Arrsel:
		assert(nd->nd_symb == '[');

		CodeDesig(nd->nd_left, ds);
		CodeAddress(ds);
		CodePExpr(nd->nd_right);
		if (nd->nd_right->nd_type->tp_size > word_size) {
			CodeCoercion(nd->nd_right->nd_type, int_type);
		}

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
			C_lae_dlb(nd->nd_left->nd_type->arr_descr, (arith) 0);
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
			CodeValue(ds, pointer_size);
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
