/* S C O P E   M E C H A N I S M */

#include	"debug.h"

#include	<alloc.h>
#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"LLlex.h"
#include	"def.h"
#include	"idf.h"
#include	"misc.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"

struct scope *GlobalScope, *PervasiveScope, *BlockScope;
struct scopelist *CurrVis;
extern int proclevel;			/* declared in declar.g */
static int sccount;

InitScope()
{
	register struct scope *sc = new_scope();
	register struct scopelist *ls = new_scopelist();

	sc->sc_def = 0;
	sc->sc_level = proclevel;
	PervasiveScope = sc;
	ls->next = 0;
	ls->sc_scope = PervasiveScope;
	ls->sc_count = ++sccount;
	CurrVis = ls;
}

open_scope()
{
	register struct scope *sc = new_scope();
	register struct scopelist *ls = new_scopelist();

	sc->sc_level = proclevel;
	ls->sc_scope = sc;
	ls->next = CurrVis;
	ls->sc_count = ++sccount;
	CurrVis = ls;
}

close_scope()
{
	/* When this procedure is called, the next visible scope is equal to
	   the statically enclosing scope
	*/

	assert(CurrentScope != 0);
	CurrVis = CurrVis->next;
}

Forward(nd, tp)
	register struct node *nd;
	register struct type *tp;
{
	/* Enter a forward reference into the current scope. This is
	 * used in pointertypes.
	 */
	register struct def *df = define(nd->nd_IDF, CurrentScope, D_FORWTYPE);
	register struct forwtype *fw_type = new_forwtype();

	fw_type->f_next = df->df_fortype;
	df->df_fortype = fw_type;

	fw_type->f_node = nd;
	fw_type->f_type = tp;
}

chk_prog_params()
{
	/* the program parameters must be global variables of some file type */
	register struct def *df = CurrentScope->sc_def;

	while( df )	{
	    if( df->df_kind & D_PARAMETER )	{
		if( !is_anon_idf(df->df_idf) )	{
		    if( df->df_type == error_type )
			 error("program parameter \"%s\" must be a global variable",
							df->df_idf->id_text);
		    else if( df->df_type->tp_fund != T_FILE )
			error("program parameter \"%s\" must have a file type",
							df->df_idf->id_text);

		    df->df_kind = D_VARIABLE;
		}
		else df->df_kind = D_ERROR;
	    }
	    df = df->df_nextinscope;
	}
}

chk_directives()
{
	/* check if all forward declarations are defined */
	register struct def *df = CurrentScope->sc_def;

	while( df )	{
		if( df->df_kind == D_FWPROCEDURE )
		     error("procedure \"%s\" not defined", df->df_idf->id_text);
		else if( df->df_kind == D_FWFUNCTION )
		      error("function \"%s\" not defined", df->df_idf->id_text);

		df = df->df_nextinscope;
	}
}
