/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* S C O P E   M E C H A N I S M */

/* $Header$ */

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

t_scope *PervasiveScope;
t_scopelist *CurrVis, *GlobalVis;
extern int proclevel;
static t_scopelist *PervVis;
extern char options[];

/* STATICALLOCDEF "scope" 10 */

/* STATICALLOCDEF "scopelist" 10 */

open_scope(scopetype)
{
	/*	Open a scope that is either open (automatic imports) or closed.
	*/
	register t_scope *sc = new_scope();
	register t_scopelist *ls = new_scopelist();
	
	assert(scopetype == OPENSCOPE || scopetype == CLOSEDSCOPE);

	sc->sc_scopeclosed = scopetype == CLOSEDSCOPE;
	sc->sc_level = proclevel;
	ls->sc_scope = sc;
	ls->sc_encl = CurrVis;
	if (scopetype == OPENSCOPE) {
		ls->sc_next = ls->sc_encl;
	}
	else	ls->sc_next = PervVis;
	CurrVis = ls;
}

t_scope *
open_and_close_scope(scopetype)
{
	t_scope *sc;

	open_scope(scopetype);
	sc = CurrentScope;
	close_scope(0);
	return sc;
}

InitScope()
{
	register t_scope *sc = new_scope();
	register t_scopelist *ls = new_scopelist();

	sc->sc_scopeclosed = 0;
	sc->sc_def = 0;
	sc->sc_level = proclevel;
	PervasiveScope = sc;
	ls->sc_next = 0;
	ls->sc_encl = 0;
	ls->sc_scope = PervasiveScope;
	PervVis = ls;
	CurrVis = ls;
}

STATIC
chk_proc(df)
	register t_def *df;
{
	/*	Called at scope closing. Check all definitions, and if one
		is a D_PROCHEAD, the procedure was not defined.
		Also check that hidden types are defined.
	*/
	while (df) {
		if (df->df_kind == D_HIDDEN) {
			error("hidden type \"%s\" not declared",
				df->df_idf->id_text);
		}
		else if (df->df_kind == D_PROCHEAD) {
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
	t_def **pdf;
{
	/*	Called at scope close. Look for all forward definitions and
		if the scope was a closed scope, give an error message for
		them, and otherwise move them to the enclosing scope.
	*/
	register t_def *df;

	while (df = *pdf) {
		if (df->df_kind == D_FORWTYPE) {
			register t_def *df1 = df;
			register t_node *nd = df->df_forw_node;

			*pdf = df->df_nextinscope;
			RemoveFromIdList(df);
			df = lookfor(nd, CurrVis, 1);
			if (! df->df_kind & (D_ERROR|D_FTYPE|D_TYPE)) {
node_error(nd, "\"%s\" is not a type", df1->df_idf->id_text);
			}
			while (nd) {
				nd->nd_type->tp_next = df->df_type;
				nd = nd->nd_right;
			}
			FreeNode(df1->df_forw_node);
			free_def(df1);
			continue;
		}
		else if (df->df_kind == D_FTYPE) {
			register t_node *nd = df->df_forw_node;

			df->df_kind = D_TYPE;
			while (nd) {
				nd->nd_type->tp_next = df->df_type;
				nd = nd->nd_right;
			}
			FreeNode(df->df_forw_node);
		}
		else if (df->df_kind & (D_FORWARD|D_FORWMODULE)) {
			/* These definitions must be found in
			   the enclosing closed scope, which of course
			   may be the scope that is now closed!
			*/
			if (scopeclosed(CurrentScope)) {
				/* Indeed, the scope was a closed
				   scope, so give error message
				*/
node_error(df->for_node, "identifier \"%s\" not declared",
df->df_idf->id_text);
				FreeNode(df->for_node);
			}
			else {
				/* This scope was an open scope.
				   Maybe the definitions are in the
				   enclosing scope?
				*/
				register t_scopelist *ls =
						nextvisible(CurrVis);
				t_def *df1 = df->df_nextinscope;
	
				if (df->df_kind == D_FORWMODULE) {
					df->for_vis->sc_next = ls;
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

Reverse(pdf)
	t_def **pdf;
{
	/*	Reverse the order in the list of definitions in a scope.
		This is neccesary because this list is built in reverse.
		Also, while we're at it, remove uninteresting definitions
		from this list.
	*/
	register t_def *df, *df1;
#define INTERESTING D_MODULE|D_PROCEDURE|D_PROCHEAD|D_VARIABLE

	df = 0;
	df1 = *pdf;

	while (df1) {
		if (df1->df_kind & INTERESTING) {
			t_def *prev = df;

			df = df1;
			df1 = df1->df_nextinscope;
			df->df_nextinscope = prev;
		}
		else	df1 = df1->df_nextinscope;
	}
	*pdf = df;
}

close_scope(flag)
	register int flag;
{
	/*	Close a scope. If "flag" is set, check for forward declarations,
		either POINTER declarations, or EXPORTs, or forward references
		to MODULES
	*/
	register t_scope *sc = CurrentScope;

	assert(sc != 0);

	if (! sc->sc_end) {
		sc->sc_end = dot2leaf(Link);
	}

	if (flag) {
		DO_DEBUG(options['S'],(print("List of definitions in currently ended scope:\n"), DumpScope(sc->sc_def)));
		if (flag & SC_CHKPROC) chk_proc(sc->sc_def);
		if (flag & SC_CHKFORW) chk_forw(&(sc->sc_def));
		if (flag & SC_REVERSE) Reverse(&(sc->sc_def));
	}
	CurrVis = enclosing(CurrVis);
}

#ifdef DEBUG
DumpScope(df)
	register t_def *df;
{
	while (df) {
		PrDef(df);
		df = df->df_nextinscope;
	}
}
#endif
