/* D E F I N I T I O N   M E C H A N I S M */

#include	"debug.h"

#include	<alloc.h>
#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"LLlex.h"
#include	"def.h"
#include	"idf.h"
#include	"main.h"
#include	"misc.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"

struct def *
MkDef(id, scope, kind)
	register struct idf *id;
	register struct scope *scope;
	long kind;
{
	/*	Create a new definition structure in scope "scope", with
	 *	id "id" and kind "kind".
	 */
	register struct def *df = new_def();

	df->df_idf = id;
	df->df_scope = scope;
	df->df_kind = kind;
	df->df_type = error_type;
	df->df_next = id->id_def;
	id->id_def = df;

	/* enter the definition in the list of definitions in this scope
	*/
	df->df_nextinscope = scope->sc_def;
	scope->sc_def = df;
	return df;
}

struct def *
define(id, scope, kind)
	register struct idf *id;
	register struct scope *scope;
	long kind;
{
	/*	Declare an identifier in a scope, but first check if it
		already has been defined.
		If so, then check for the cases in which this is legal,
		and otherwise give an error message.
	*/
	register struct def *df;

	if( df = lookup(id, scope, 0L) )	{
		if (df->df_kind == D_INUSE) {
			if( kind != D_INUSE ) {
			    error("\"%s\" already used in this block",
							id->id_text);
			}
			return MkDef(id, scope, kind);
		}
		if (df->df_kind == D_ERROR ) {
			/* used in forward references */
			df->df_kind = kind;
			return df;
		}
		/* other cases fit in an int (assume at least 2 bytes) */
		switch((int) df->df_kind )	{

		    case D_LABEL :
			/* generate error message somewhere else */
			return NULLDEF;

		    case D_PARAMETER :
			if( kind == D_VARIABLE )
			/* program parameter declared as variable */
				return df;
			break;

		    case D_FORWTYPE :
			if( kind == D_FORWTYPE ) return df;
			if( kind == D_TYPE )	{
			/* forward reference resolved */
				df->df_kind = D_FTYPE;
				return df;
			}
			else
				error("identifier \"%s\" must be a type",
							id->id_text);
			return NULLDEF;

		    case D_FWPROCEDURE :
			if( kind == D_PROCEDURE ) return df;
			error("procedure identification \"%s\" expected",
								id->id_text);
			return NULLDEF;

		    case D_FWFUNCTION :
			if( kind == D_FUNCTION ) return df;
			error("function identification \"%s\" expected",
								id->id_text);
			return NULLDEF;

		}
		if( kind != D_ERROR )
			/* avoid spurious error messages */
		        error("identifier \"%s\" already declared",id->id_text);

		return NULLDEF;
	}

	return MkDef(id, scope, kind);
}

DoDirective(directive, nd, tp, scl, function)
	struct idf *directive;
	struct node *nd;
	struct type *tp;
	struct scopelist *scl;
{
	long kind;			/* kind of directive */
	int inp;			/* internal or external name */
	int ext = 0;		/* directive = EXTERN */
	struct def *df = lookup(directive, PervasiveScope, D_INUSE);

	if( !df )	{
		if( !is_anon_idf(directive) )
			node_error(nd, "\"%s\" unknown directive",
							directive->id_text);
		return;
	}

	if (df->df_kind == D_FORWARD) {
		kind = function ? D_FWFUNCTION : D_FWPROCEDURE;
		inp = (proclevel > 1);
	}
	else if (df->df_kind == D_EXTERN) {
		kind = function ? D_FUNCTION : D_PROCEDURE;
		inp = 0;
		ext = 1;
	}
	else {
		node_error(nd, "\"%s\" unknown directive",
						directive->id_text);
		return;
	}

	if( df = define(nd->nd_IDF, CurrentScope, kind) )	{
		if( df->df_kind != kind )	{
			/* identifier already forward declared */
			node_error(nd, "\"%s\" already forward declared",
							nd->nd_IDF->id_text);
			return;
		}

		df->df_type = tp;
		df->prc_vis = scl;
		df->prc_name = gen_proc_name(nd->nd_IDF, inp);
		if( ext ) {
			if (!(df->df_flags & D_EXTERNAL) && proclevel > 1)
				tp->prc_nbpar -= pointer_size;
			/* was added for static link which is not needed now.
			   But make sure this is done only once (look at the
			   D_EXTERNAL flag).
			*/
			df->df_flags |= D_EXTERNAL;
		}
		df->df_flags |= D_SET;
	}
}

struct def *
DeclProc(nd, tp, scl)
	register struct node *nd;
	struct type *tp;
	register struct scopelist *scl;
{
	register struct def *df;

	if( df = define(nd->nd_IDF, CurrentScope, D_PROCEDURE) )	{
		df->df_flags |= D_SET;
		if( df->df_kind == D_FWPROCEDURE )	{
			df->df_kind = D_PROCEDURE;	/* identification */

			/* Simulate a call to open_scope(), which has already
			 * been performed in the forward declaration.
			 */
			CurrVis = df->prc_vis;

			if( tp->prc_params )
				node_error(nd,
				  "\"%s\" already declared",
							nd->nd_IDF->id_text);
		}
		else	{	/* normal declaration */
			df->df_type = tp;
			df->prc_name = gen_proc_name(nd->nd_IDF, (proclevel>1));
			/* simulate open_scope() */
			CurrVis = df->prc_vis = scl;
		}
		routine_label(df);
	}
	else CurrVis = scl;		/* simulate open_scope() */

	return df;
}

struct def *
DeclFunc(nd, tp, scl)
	register struct node *nd;
	struct type *tp;
	register struct scopelist *scl;
{
	register struct def *df;

	if( df = define(nd->nd_IDF, CurrentScope, D_FUNCTION) )	{
	    df->df_flags &= ~D_SET;
	    if( df->df_kind == D_FUNCTION )	{	/* declaration */
		if( !tp )	{
			node_error(nd, "\"%s\" illegal function declaration",
							nd->nd_IDF->id_text);
			tp = construct_type(T_FUNCTION, error_type);
		}
		/* simulate open_scope() */
		CurrVis = df->prc_vis = scl;
		df->df_type = tp;
		df->prc_name = gen_proc_name(nd->nd_IDF, (proclevel > 1));
	    }
	    else	{			/* identification */
		assert(df->df_kind == D_FWFUNCTION);

		df->df_kind = D_FUNCTION;
		CurrVis = df->prc_vis;

		if( tp )
			node_error(nd,
				   "\"%s\" already declared",
				   nd->nd_IDF->id_text);

	    }
	    routine_label(df);
	}
	else CurrVis = scl;			/* simulate open_scope() */

	return df;
}

EndFunc(df)
	register struct def *df;
{
	/* assignment to functionname is illegal outside the functionblock */
	df->prc_res = 0;

	/* Give the error about assignment as soon as possible. The
	 * |= assignment inhibits a warning in the main procedure.
	 */
	if( !(df->df_flags & D_SET) ) {
		error("function \"%s\" not assigned",df->df_idf->id_text);
		df->df_flags |= D_SET;
	}
}

EndBlock(block_df)
	register struct def *block_df;
{
	register struct def *tmp_def = CurrentScope->sc_def;
	register struct def *df;

	while( tmp_def ) {
	    df = tmp_def;
	    	/* The length of a usd_def chain is at most 1.
		 * The while is just defensive programming.
		 */
	    while( df->df_kind & D_INUSE )
		df = df->usd_def;

	    if( !is_anon_idf(df->df_idf)
		    && (df->df_scope == CurrentScope) ) {
		if( !(df->df_kind & (D_ENUM|D_LABEL|D_ERROR)) ) {
		    if( !(df->df_flags & D_USED) ) {
			if( !(df->df_flags & D_SET) ) {
			    warning("\"%s\" neither set nor used in \"%s\"",
				df->df_idf->id_text, block_df->df_idf->id_text);
			}
			else {
			    warning("\"%s\" unused in \"%s\"",
				df->df_idf->id_text, block_df->df_idf->id_text);
			}
		    }
		    else if( !(df->df_flags & D_SET) ) {
			if( !(df->df_flags & D_LOOPVAR) )
			    warning("\"%s\" not set in \"%s\"",
				df->df_idf->id_text, block_df->df_idf->id_text);
		    }
		}

	    }
	    tmp_def = tmp_def->df_nextinscope;
	}
}
