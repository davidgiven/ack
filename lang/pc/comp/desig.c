/* D E S I G N A T O R   E V A L U A T I O N */

/*	Code generation for designators.
	This file contains some routines that generate code common to address
	as well as value computations, and leave a description in a "desig"
	structure. It also contains routines to load an address, load a value
	or perform a store.
*/

#include	"debug.h"

#include	<assert.h>
#include	<em.h>

#include	"LLlex.h"
#include	"def.h"
#include	"desig.h"
#include	"main.h"
/* next line DEBUG */
#include	"idf.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"

struct desig	InitDesig = {DSG_INIT, 0, 0, NULLDEF, 0};
struct withdesig *WithDesigs;


STATIC int
properly(ds, size, al)
	register struct desig *ds;
	arith size;
{
	/*	Check if it is allowed to load or store the value indicated
		by "ds" with LOI/STI.
		- if the size is not either a multiple or a dividor of the
		  wordsize, then not.
		- if the alignment is at least "word" then OK.
		- if size is dividor of word_size and alignment >= size then OK.
		- otherwise check alignment of address. This can only be done
		  with DSG_FIXED.
	*/

	arith szmodword = size % word_size;	/* 0 if multiple of wordsize */
	arith wordmodsz = word_size % size;	/* 0 if dividor of wordsize */

	if( szmodword && wordmodsz ) return 0;
	if( al >= word_align ) return 1;
	if( szmodword && al >= szmodword ) return 1;

	return ds->dsg_kind == DSG_FIXED &&
	       ((! szmodword && ds->dsg_offset % word_align == 0) ||
		(! wordmodsz && ds->dsg_offset % size == 0));
}

CodeCopy(lhs, rhs, sz, psize)
	register struct desig *lhs, *rhs;
	arith sz, *psize;
{
	struct desig l, r;

	l = *lhs;
	r = *rhs;
	*psize -= sz;
	lhs->dsg_offset += sz;
	rhs->dsg_offset += sz;
	CodeAddress(&r);
	C_loi(sz);
	CodeAddress(&l);
	C_sti(sz);
}

CodeMove(rhs, left, rtp)
	register struct desig *rhs;
	register struct node *left;
	struct type *rtp;
{
	struct desig dsl;
	register struct desig *lhs = &dsl;
	register struct type *ltp = left->nd_type;

	dsl = InitDesig;
	/*	Generate code for an assignment. Testing of type
		compatibility and the like is already done.
		Go through some (considerable) trouble to see if
		a BLM can be generated.
	*/

	switch( rhs->dsg_kind )	{
	case DSG_LOADED:
		CodeDesig(left, lhs);
		if( rtp->tp_fund == T_STRINGCONST )	{
			CodeAddress(lhs);
			C_blm(lhs->dsg_packed ? ltp->tp_psize : ltp->tp_size);
			return;
		}
		CodeStore(lhs, ltp);
		return;

	case DSG_PLOADED:
	case DSG_PFIXED:
		CodeAddress(rhs);
		CodeValue(rhs, rtp);
		CodeDStore(left);
		return;

	case DSG_FIXED:	{
		arith tpsize;

		CodeDesig(left, lhs);
		tpsize = lhs->dsg_packed ? ltp->tp_psize : ltp->tp_size;
		if( lhs->dsg_kind == DSG_FIXED &&
		    lhs->dsg_offset % word_size == rhs->dsg_offset % word_size
		  )	{
			arith size = tpsize;

			if( size > 6 * word_size )	{
				/*	Do a block move
				*/
				struct desig l, r;

				l = *lhs;
				r = *rhs;
				CodeAddress(&r);
				CodeAddress(&l);
				C_blm(size);
			}
			else	{
				register arith sz;

				for( sz = 2 * word_size; sz; sz -= word_size) {
					while( size >= sz )
					/*	Then copy dwords, words.
						Depend on peephole optimizer
					*/
					CodeCopy(lhs, rhs, sz, &size);
				}
			}
			return;
		}
		if( lhs->dsg_kind == DSG_PLOADED ||
		    lhs->dsg_kind == DSG_INDEXED )	{
			CodeAddress(lhs);
		}
	}
	default:
		crash("(CodeMove)");
		/*NOTREACHED*/
	}
}

CodeValue(ds, tp)
	register struct desig *ds;
	register struct type *tp;
{
	/*	Generate code to load the value of the designator described
		in "ds"
	*/
	arith size = ds->dsg_packed ? tp->tp_psize : tp->tp_size;
	int algn = ds->dsg_packed ? tp->tp_palign : tp->tp_align;

	switch( ds->dsg_kind )	{
	case DSG_LOADED:
		return;

	case DSG_FIXED:
		if( ds->dsg_offset % word_size == 0 ) {
			if ( size == word_size ) {
				if( ds->dsg_name )
					C_loe_dnam(ds->dsg_name, ds->dsg_offset);
				else
					C_lol(ds->dsg_offset);
				break;
			} else if ( size == word_size * 2) {
				if( ds->dsg_name )
					C_lde_dnam(ds->dsg_name, ds->dsg_offset);
				else
					C_ldl(ds->dsg_offset);
				break;
			}
		}
		/* Fall through */
	case DSG_PLOADED:
	case DSG_PFIXED:
		if( properly(ds, size, algn) )	{
			CodeAddress(ds);
			C_loi(size);
			break;
		}
		crash("(CodeValue)");
		break;

	case DSG_INDEXED:
		C_lar(word_size);
		break;

	default:
		crash("(CodeValue)");
		/*NOTREACHED*/
	}

	if (size < word_size && tp->tp_fund == T_SUBRANGE &&
	    BaseType(tp)->tp_fund == T_INTEGER && tp->sub_lb < 0) {
		C_loc(size);
		C_loc(word_size);
		C_cii();
	}
	ds->dsg_kind = DSG_LOADED;
}

CodeStore(ds, tp)
	register struct desig *ds;
	register struct type *tp;
{
	/*	Generate code to store the value on the stack in the designator
		described in "ds"
	*/
	struct desig save;
	arith size = ds->dsg_packed ? tp->tp_psize : tp->tp_size;
	int algn = ds->dsg_packed ? tp->tp_palign : tp->tp_align;

	save = *ds;
	
	switch( ds->dsg_kind )	{
	case DSG_FIXED:
		if( ds->dsg_offset % word_size == 0 ) {
			if ( size == word_size ) {
				if( ds->dsg_name )
					C_ste_dnam(ds->dsg_name, ds->dsg_offset);
				else
					C_stl(ds->dsg_offset);
				break;
			} else if ( size == word_size * 2) {
				if( ds->dsg_name )
					C_sde_dnam(ds->dsg_name, ds->dsg_offset);
				else
					C_sdl(ds->dsg_offset);
				break;
			}
		}
		/* Fall through */
	case DSG_PLOADED:
	case DSG_PFIXED:
		CodeAddress(&save);
		if( properly(ds, size, algn) )	{
			C_sti(size);
			break;
		}
		crash("(CodeStore)");
		break;

	case DSG_INDEXED:
		C_sar(word_size);
		break;

	default:
		crash("(CodeStore)");
		/*NOTREACHED*/
	}

	ds->dsg_kind = DSG_INIT;
}

CodeAddress(ds)
	register struct desig *ds;
{
	/*	Generate code to load the address of the designator described
	   	in "ds"
	*/

	switch( ds->dsg_kind )	{
	case DSG_PLOADED:
		if( ds->dsg_offset )
			C_adp(ds->dsg_offset);
		break;

	case DSG_FIXED:
		if( ds->dsg_name )	{
			C_lae_dnam(ds->dsg_name, ds->dsg_offset);
			break;
		}
		C_lal(ds->dsg_offset);
		if( ds->dsg_def )
			ds->dsg_def->df_flags |= D_NOREG;
		break;
		
	case DSG_PFIXED:
		if ( word_size == pointer_size ) {
			if( ds->dsg_name )
				C_loe_dnam(ds->dsg_name, ds->dsg_offset);
			else
				C_lol(ds->dsg_offset);
			break;
		} else {
			if( ds->dsg_name )
				C_lde_dnam(ds->dsg_name, ds->dsg_offset);
			else
				C_ldl(ds->dsg_offset);
			break;
		}

	case DSG_INDEXED:
		C_aar(word_size);
		break;

	default:
		crash("(CodeAddress)");
		/*NOTREACHED*/
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

	if( ds->dsg_kind == DSG_INIT )	{
		/* In a WITH statement. We must find the designator in the
		   WITH statement, and act as if the field is a selection
		   of this designator.
		   So, first find the right WITH statement, which is the
		   first one of the proper record type, which is
		   recognized by its scope indication.
		*/
		register struct withdesig *wds = WithDesigs;

		assert(wds != 0);

		while( wds->w_scope != df->df_scope )	{
			wds = wds->w_next;
			assert(wds != 0);
		}

		/* Found it. Now, act like it was a selection.
		*/
		*ds = wds->w_desig;
		assert(ds->dsg_kind == DSG_PFIXED);
	}

	switch( ds->dsg_kind )	{
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

	ds->dsg_packed = df->fld_flags & F_PACKED;
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

	assert(ds->dsg_kind == DSG_INIT);

	if( df->var_name )	{
		/* this variable has been given a name, so it is global.
		   It is directly accessible.
		*/
		ds->dsg_name = df->var_name;
		ds->dsg_offset = 0;
		ds->dsg_kind = DSG_FIXED;
		return;
	}

	if( sc->sc_level != proclevel )	{
		/* the variable is local to a statically enclosing procedure.
		*/
		assert(proclevel > sc->sc_level);

		df->df_flags |= D_NOREG;
		if( df->df_flags & (D_VARPAR|D_VALPAR) )	{
			/* value or var parameter
			*/
			C_lxa((arith) (proclevel - sc->sc_level));
			if( (df->df_flags & D_VARPAR) ||
			    IsConformantArray(df->df_type) )	{
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
		else
			C_lxl((arith) (proclevel - sc->sc_level));

		ds->dsg_kind = DSG_PLOADED;
		ds->dsg_offset = df->var_off;
		return;
	}

	/* Now, finally, we have a local variable or a local parameter
	*/
	if( (df->df_flags & D_VARPAR) || IsConformantArray(df->df_type) )
		/* a var parameter; address directly accessible. */
		ds->dsg_kind = DSG_PFIXED;
	else
		ds->dsg_kind = DSG_FIXED;

	ds->dsg_offset = df->var_off;
	ds->dsg_def = df;
}

CodeBoundDesig(df, ds)
	register struct def *df;
	register struct desig *ds;
{
	/* Generate code for the lower- and upperbound of a conformant array */

	assert(ds->dsg_kind == DSG_INIT);

	if( df->df_scope->sc_level < proclevel )	{
		C_lxa((arith) (proclevel - df->df_scope->sc_level));
		C_lof(df->bnd_type->arr_cfdescr);
		if( df->df_kind == D_UBOUND )	{
			C_lxa((arith) (proclevel - df->df_scope->sc_level));
			C_lof(df->bnd_type->arr_cfdescr+word_size);
			C_adi(word_size);
		}
	}
	else	{
		C_lol(df->bnd_type->arr_cfdescr);
		if( df->df_kind == D_UBOUND )	{
			C_lol(df->bnd_type->arr_cfdescr+word_size);
			C_adi(word_size);
		}
	}

	ds->dsg_kind = DSG_LOADED;
}

CodeFuncDesig(df, ds)
	register struct def *df;
	register struct desig *ds;
{
	/* generate code to store the function result */

	if( df->df_scope->sc_level + 1 < proclevel )	{
		/* Assignment to function-identifier in the declaration-part of
		   the function (i.e. in the statement-part of a nested function
		   or procedure).
		*/
		if( !options['R'] ) {
			C_loc((arith)1);
			C_lxl((arith) (proclevel - df->df_scope->sc_level - 1));
			C_adp(df->prc_bool);
			C_sti(int_size);
		}

		C_lxl((arith) (proclevel - df->df_scope->sc_level - 1));
		ds->dsg_kind = DSG_PLOADED;
	}
	else	{
		/* Assignment to function-identifier in the statement-part of
		   the function.
		*/
		if( !options['R'] ) {
			C_loc((arith)1);
			C_stl(df->prc_bool);
		}

		ds->dsg_kind = DSG_FIXED;
	}
	assert(df->prc_res < 0);
	ds->dsg_offset = df->prc_res;
}

CodeDesig(nd, ds)
	register struct node *nd;
	register struct desig *ds;
{
	/*	Generate code for a designator. Use divide and conquer
		principle
	*/
	register struct def *df;

	switch( nd->nd_class )	{	/* Divide */
	case Def:
		df = nd->nd_def;

		switch( (int) df->df_kind )	{
		case D_FIELD:
			CodeFieldDesig(df, ds);
			break;

		case D_VARIABLE:
			CodeVarDesig(df, ds);
			break;

		case D_LBOUND:
		case D_UBOUND:
			CodeBoundDesig(df, ds);
			break;

		case D_FUNCTION:
			CodeFuncDesig(df, ds);
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

	case Arrsel:	{
		struct type *tp;

		assert(nd->nd_symb == '[');

		CodeDesig(nd->nd_left, ds);
		CodeAddress(ds);
		CodePExpr(nd->nd_right);

		/* Now load address of descriptor
		*/
		tp = nd->nd_left->nd_type;
		if( IsConformantArray(tp) )	{
			if( tp->arr_sclevel < proclevel )	{
				C_lxa((arith) (proclevel - tp->arr_sclevel));
				C_adp(tp->arr_cfdescr);
			}
			else
				C_lal(tp->arr_cfdescr);
		}
		else
			C_lae_dlb(tp->arr_ardescr, (arith) 0);

		if( options['A'] ) {
			C_cal("_rcka");
		}
		ds->dsg_kind = DSG_INDEXED;
		ds->dsg_packed = IsPacked(tp);
		break;
	}

	case Arrow:
		assert(nd->nd_symb == '^');

		if( nd->nd_right->nd_type->tp_fund == T_FILE )	{
			CodeDAddress(nd->nd_right);
			C_cal("_wdw");
			C_asp(pointer_size);
			C_lfr(pointer_size);
			ds->dsg_kind = DSG_PLOADED;
			ds->dsg_packed = 1;
			break;
		}

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
