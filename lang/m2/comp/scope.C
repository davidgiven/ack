/* S C O P E   M E C H A N I S M */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

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

/* STATICALLOCDEF "scope" */

/* STATICALLOCDEF "scopelist" */

open_scope(scopetype)
{
	/*	Open a scope that is either open (automatic imports) or closed.
	*/
	register struct scope *sc = new_scope();
	register struct scopelist *ls = new_scopelist();
	
	assert(scopetype == OPENSCOPE || scopetype == CLOSEDSCOPE);

	clear((char *) sc, sizeof (struct scope));
	sc->sc_scopeclosed = scopetype == CLOSEDSCOPE;
	sc->sc_level = proclevel;
	if (scopetype == OPENSCOPE) {
		ls->next = CurrVis;
	}
	else	ls->next = PervVis;
	ls->sc_scope = sc;
	ls->sc_encl = CurrVis;
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

STATIC
chk_proc(df)
	register struct def *df;
{
	/*	Called at scope closing. Check all definitions, and if one
		is a D_PROCHEAD, the procedure was not defined
	*/
	while (df) {
		if (df->df_kind == D_PROCHEAD) {
			/* A not defined procedure
			*/
node_error(df->for_node, "procedure \"%s\" not defined", df->df_idf->id_text);
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
	while (*pdf) {
		if ((*pdf)->df_kind & (D_FORWARD|D_FORWMODULE)) {
			/* These definitions must be found in
			   the enclosing closed scope, which of course
			   may be the scope that is now closed!
			*/
			struct def *df1 = (*pdf)->df_nextinscope;

			if (scopeclosed(CurrentScope)) {
				/* Indeed, the scope was a closed
				   scope, so give error message
				*/
node_error((*pdf)->for_node, "identifier \"%s\" has not been declared",
(*pdf)->df_idf->id_text);
				FreeNode((*pdf)->for_node);
				pdf = &(*pdf)->df_nextinscope;
			}
			else {	/* This scope was an open scope.
				   Maybe the definitions are in the
				   enclosing scope?
				*/
				struct scopelist *ls;

				ls = nextvisible(CurrVis);
				if ((*pdf)->df_kind == D_FORWMODULE) {
					(*pdf)->for_vis->next = ls;
				}
				(*pdf)->df_nextinscope = ls->sc_scope->sc_def;
				ls->sc_scope->sc_def = *pdf;
				(*pdf)->df_scope = ls->sc_scope;
				*pdf = df1;
			}
		}
		else	pdf = &(*pdf)->df_nextinscope;
	}
}

STATIC
rem_forwards(fo)
	struct forwards *fo;
{
	/*	When closing a scope, all forward references must be resolved
	*/
	register struct forwards *f;
	register struct def *df;

	while (f = fo) {
		df = lookfor(&(f->fo_tok), CurrVis, 1);
		if (!(df->df_kind & (D_TYPE|D_ERROR))) {
			node_error(&(f->fo_tok), "identifier \"%s\" not a type",
			      df->df_idf->id_text);
		}
		*(f->fo_ptyp) = df->df_type;
		fo = f->next;
		free_forwards(f);
	}
}

Reverse(pdf)
	register struct def **pdf;
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
		if (df1->df_kind & INTERESTING) break;
		df1 = df1->df_nextinscope;
	}

	if (!(*pdf = df1)) return;

	while (df1) {
		*pdf = df1;
		df1 = df1->df_nextinscope;
		while (df1) {
			if (df1->df_kind & INTERESTING) break;
			df1 = df1->df_nextinscope;
		}
		(*pdf)->df_nextinscope = df;
		df = *pdf;
	}
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
