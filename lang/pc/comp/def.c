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
{
	/*	Declare an identifier in a scope, but first check if it
		already has been defined.
		If so, then check for the cases in which this is legal,
		and otherwise give an error message.
	*/
	register struct def *df;

	if( df = lookup(id, scope) )	{
		switch( df->df_kind )	{

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

		    case D_ERROR :
			/* used in forward references */
			df->df_kind = kind;
			return df;
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
	int kind;			/* kind of directive */
	int inp;			/* internal or external name */
	int ext = 0;		/* directive = EXTERN */
	struct def *df = lookup(directive, PervasiveScope);

	if( !df )	{
		if( !is_anon_idf(directive) )
			node_error(nd, "\"%s\" unknown directive",
							directive->id_text);
		return;
	}

	switch( df->df_kind)	{
		case D_FORWARD:
			kind = function ? D_FWFUNCTION : D_FWPROCEDURE;
			inp = (proclevel > 1);
			break;

		case D_EXTERN:
			kind = function ? D_FUNCTION : D_PROCEDURE;
			inp = 0;
			ext = 1;
			break;

		default:
			crash("(DoDirective)");
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
		if( ext ) df->df_flags |= D_EXTERNAL;
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
		if( df->df_kind == D_FWPROCEDURE )	{
			df->df_kind = D_PROCEDURE;	/* identification */

			/* Simulate a call to open_scope(), which has already
			 * been performed in the forward declaration.
			 */
			CurrVis = df->prc_vis;

			if( tp->prc_params )
				node_error(nd,
				  "procedure identification \"%s\" expected",
							nd->nd_IDF->id_text);
		}
		else	{	/* normal declaration */
			df->df_type = tp;
			df->prc_name = gen_proc_name(nd->nd_IDF, (proclevel>1));
			/* simulate open_scope() */
			CurrVis = df->prc_vis = scl;
		}
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
	    if( df->df_kind == D_FUNCTION )	{	/* declaration */
		if( !tp )	{
			node_error(nd, "\"%s\" illegal function declaration",
							nd->nd_IDF->id_text);
			tp = error_type;
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
				   "function identification \"%s\" expected",
				   nd->nd_IDF->id_text);

	    }
	}
	else CurrVis = scl;			/* simulate open_scope() */

	return df;
}
