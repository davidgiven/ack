/* S C O P E   M E C H A N I S M */

static char *RcsId = "$Header$";

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
#include	"debug.h"

static int maxscope;		/* maximum assigned scope number */

struct scope *CurrentScope, *GlobalScope;

/* STATICALLOCDEF "scope" */

open_scope(scopetype, scope)
{
	/*	Open a scope that is either open (automatic imports) or closed.
		A closed scope is handled by adding an extra entry to the list
		with scope number 0. This has two purposes: it makes scope 0
		visible, and it marks the end of a visibility list.
		Scope 0 is the pervasive scope, the one that is always visible.
		A disadvantage of this method is that we cannot open scope 0
		explicitly.
	*/
	register struct scope *sc = new_scope();
	register struct scope *sc1;

	sc->sc_scope = scope == 0 ? ++maxscope : scope;
	sc->sc_forw = 0;
	sc->sc_def = 0;
	assert(scopetype == OPENSCOPE || scopetype == CLOSEDSCOPE);
	DO_DEBUG(1, debug("Opening a %s scope",
			scopetype == OPENSCOPE ? "open" : "closed"));
	sc1 = CurrentScope;
	if (scopetype == CLOSEDSCOPE) {
		sc1 = new_scope();
		sc1->sc_scope = 0;		/* Pervasive scope nr */
		sc1->sc_forw = 0;
		sc1->sc_def = 0;
		sc1->next = CurrentScope;
	}
	sc->next = sc1;
	CurrentScope = sc;
}

init_scope()
{
	register struct scope *sc = new_scope();

	sc->sc_scope = 0;
	sc->sc_forw = 0;
	sc->sc_def = 0;
	CurrentScope = sc;
}

int
uniq_scope()
{
	return ++maxscope;
}

struct forwards {
	struct forwards *next;
	struct node fo_tok;
	struct type **fo_ptyp;
};

/* STATICALLOCDEF "forwards" */

Forward(tk, ptp)
	struct token *tk;
	struct type **ptp;
{
	/*	Enter a forward reference into a list belonging to the
		current scope. This is used for POINTER declarations, which
		may have forward references that must howewer be declared in the
		same scope.
	*/
	register struct forwards *f = new_forwards();

	f->fo_tok.nd_token = *tk;
	f->fo_ptyp = ptp;
	f->next = CurrentScope->sc_forw;
	CurrentScope->sc_forw = f;
}

close_scope(flag)
{
	/*	Close a scope. If "flag" is set, check for forward declarations,
		either POINTER declarations, or EXPORTs, or forward references
		to MODULES
	*/
	register struct scope *sc = CurrentScope;
	register struct def *df, *dfback = 0;

	assert(sc != 0);
	DO_DEBUG(1, debug("Closing a scope"));

	if (flag) {
		if (sc->sc_forw) rem_forwards(sc->sc_forw);
		df = sc->sc_def;
		while(df) {
			if (flag & SC_CHKPROC) {
				if (df->df_kind == D_PROCHEAD) {
					/* A not defined procedure
					*/
node_error(df->for_node, "procedure \"%s\" not defined", df->df_idf->id_text);
					FreeNode(df->for_node);
				}
			}
			if ((flag & SC_CHKFORW) && 
			    df->df_kind & (D_FORWARD|D_FORWMODULE)) {
				/* These definitions must be found in
				   the enclosing closed scope, which of course
				   may be the scope that is now closed!
				*/
				struct def *df1 = df->df_nextinscope;

				if (scopeclosed(CurrentScope)) {
					/* Indeed, the scope was a closed
					   scope, so give error message
					*/
node_error(df->for_node, "identifier \"%s\" not declared", df->df_idf->id_text);
					FreeNode(df->for_node);
					dfback = df;
				}
				else {
					/* This scope was an open scope.
					   Maybe the definitions are in the
					   enclosing scope?
					*/
					struct scope *sc;

					sc = enclosing(CurrentScope);
					df->df_nextinscope = sc->sc_def;
					sc->sc_def = df;
					df->df_scope = sc->sc_scope;
					if (dfback) dfback->df_nextinscope = df1;
					else sc->sc_def = df1;
				}
				df = df1;
			}
			else {
				dfback = df;
				df = df->df_nextinscope;
			}
		}
	}

	if (sc->next && (sc->next->sc_scope == 0)) {
		sc = sc->next;
	}
	CurrentScope = sc->next;
}

static
rem_forwards(fo)
	struct forwards *fo;
{
	/*	When closing a scope, all forward references must be resolved
	*/
	register struct forwards *f;
	register struct def *df;
	struct def *lookfor();

	while (f = fo) {
		df = lookfor(&(f->fo_tok), CurrentScope, 1);
		if (!(df->df_kind & (D_TYPE|D_HTYPE|D_ERROR))) {
			node_error(&(f->fo_tok), "identifier \"%s\" not a type",
			      df->df_idf->id_text);
		}
		*(f->fo_ptyp) = df->df_type;
		fo = f->next;
		free_forwards(f);
	}
}
