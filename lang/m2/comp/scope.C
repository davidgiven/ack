/* S C O P E   M E C H A N I S M */

#include	"debug.h"

#include	<assert.h>
#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"LLlex.h"
#include	"idf.h"
#include	"scope.h"
#include	"type.h"
#include	"def.h"
#include	"node.h"

struct scope *PervasiveScope, *GlobalScope;
struct scopelist *CurrVis;
extern int proclevel;
static struct scopelist *PervVis;
extern char options[];

/* STATICALLOCDEF "scope" 10 */

/* STATICALLOCDEF "scopelist" 10 */

open_scope(scopetype)
{
	/*	Open a scope that is either open (automatic imports) or closed.
	*/
	register struct scope *sc = new_scope();
	register struct scopelist *ls = new_scopelist();
	
	assert(scopetype == OPENSCOPE || scopetype == CLOSEDSCOPE);

	sc->sc_scopeclosed = scopetype == CLOSEDSCOPE;
	sc->sc_level = proclevel;
	ls->sc_scope = sc;
	ls->sc_encl = CurrVis;
	if (scopetype == OPENSCOPE) {
		ls->next = ls->sc_encl;
	}
	else	ls->next = PervVis;
	CurrVis = ls;
}

InitScope()
{
	register struct scope *sc = new_scope();
	register struct scopelist *ls = new_scopelist();

	sc->sc_scopeclosed = 0;
	sc->sc_forw = 0;
	sc->sc_def = 0;
	sc->sc_level = proclevel;
	PervasiveScope = sc;
	ls->next = 0;
	ls->sc_encl = 0;
	ls->sc_scope = PervasiveScope;
	PervVis = ls;
	CurrVis = ls;
}

struct forwards {
	struct forwards *next;
	struct node *fo_tok;
	struct type *fo_ptyp;
};

/* STATICALLOCDEF "forwards" 5 */

Forward(tk, ptp)
	struct node *tk;
	struct type *ptp;
{
	/*	Enter a forward reference into a list belonging to the
		current scope. This is used for POINTER declarations, which
		may have forward references that must howewer be declared in the
		same scope.
	*/
	register struct forwards *f = new_forwards();
	register struct scope *sc = CurrentScope;

	f->fo_tok = tk;
	f->fo_ptyp = ptp;
	f->next = sc->sc_forw;
	sc->sc_forw = f;
}

STATIC
chk_proc(df)
	register struct def *df;
{
	/*	Called at scope closing. Check all definitions, and if one
		is a D_PROCHEAD, the procedure was not defined.
	*/
	while (df) {
		if (df->df_kind == D_PROCHEAD) {
			/* A not defined procedure
			*/
			error("procedure \"%s\" not defined",
				df->df_idf->id_text);
			FreeNode(df->for_node);
		}
		df = df->df_nextinscope;
	}
}

STATIC
chk_forw(pdf)
	register struct def **pdf;
{
	/*	Called at scope close. Look for all forward definitions and
		if the scope was a closed scope, give an error message for
		them, and otherwise move them to the enclosing scope.
	*/
	register struct def *df;

	while (df = *pdf) {
		if (df->df_kind & (D_FORWARD|D_FORWMODULE)) {
			/* These definitions must be found in
			   the enclosing closed scope, which of course
			   may be the scope that is now closed!
			*/
			if (scopeclosed(CurrentScope)) {
				/* Indeed, the scope was a closed
				   scope, so give error message
				*/
node_error(df->for_node, "identifier \"%s\" has not been declared",
df->df_idf->id_text);
				FreeNode(df->for_node);
			}
			else {
				/* This scope was an open scope.
				   Maybe the definitions are in the
				   enclosing scope?
				*/
				register struct scopelist *ls =
						nextvisible(CurrVis);
				struct def *df1 = df->df_nextinscope;
	
				if (df->df_kind == D_FORWMODULE) {
					df->for_vis->next = ls;
				}
				df->df_nextinscope = ls->sc_scope->sc_def;
				ls->sc_scope->sc_def = df;
				df->df_scope = ls->sc_scope;
				*pdf = df1;
				continue;
			}
		}
		pdf = &df->df_nextinscope;
	}
}

STATIC
rem_forwards(fo)
	register struct forwards *fo;
{
	/*	When closing a scope, all forward references must be resolved
	*/
	register struct def *df;

	if (fo->next) rem_forwards(fo->next);
	df = lookfor(fo->fo_tok, CurrVis, 0);
	if (! is_type(df)) {
		node_error(fo->fo_tok,
			   "identifier \"%s\" does not represent a type",
			   df->df_idf->id_text);
	}
	fo->fo_ptyp->next = df->df_type;
	free_forwards(fo);
}

Reverse(pdf)
	struct def **pdf;
{
	/*	Reverse the order in the list of definitions in a scope.
		This is neccesary because this list is built in reverse.
		Also, while we're at it, remove uninteresting definitions
		from this list.
	*/
	register struct def *df, *df1;
#define INTERESTING D_MODULE|D_PROCEDURE|D_PROCHEAD|D_VARIABLE

	df = 0;
	df1 = *pdf;

	while (df1) {
		if (df1->df_kind & INTERESTING) {
			struct def *prev = df;

			df = df1;
			df1 = df1->df_nextinscope;
			df->df_nextinscope = prev;
		}
		else	df1 = df1->df_nextinscope;
	}
	*pdf = df;
}

close_scope(flag)
{
	/*	Close a scope. If "flag" is set, check for forward declarations,
		either POINTER declarations, or EXPORTs, or forward references
		to MODULES
	*/
	register struct scope *sc = CurrentScope;

	assert(sc != 0);

	if (flag) {
		if (sc->sc_forw) rem_forwards(sc->sc_forw);
		DO_DEBUG(options['S'], PrScopeDef(sc->sc_def));
		if (flag & SC_CHKPROC) chk_proc(sc->sc_def);
		if (flag & SC_CHKFORW) chk_forw(&(sc->sc_def));
		if (flag & SC_REVERSE) Reverse(&(sc->sc_def));
	}
	CurrVis = enclosing(CurrVis);
}

#ifdef DEBUG
PrScopeDef(df)
	register struct def *df;
{
	print("List of definitions in currently ended scope:\n");
	while (df) {
		PrDef(df);
		df = df->df_nextinscope;
	}
}
#endif
