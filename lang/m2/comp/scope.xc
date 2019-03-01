/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* S C O P E   M E C H A N I S M */

/* $Id$ */

#include 	"parameters.h"
#include	"debug.h"

#include	<assert.h>
#include	"alloc.h"
#include	"em_arith.h"
#include	"em_label.h"

#include	"LLlex.h"
#include	"idf.h"
#include	"scope.h"
#include	"type.h"
#include	"def.h"
#include	"node.h"
#include	"lookup.h"
#include	"error.h"

t_scope *PervasiveScope;
t_scopelist *CurrVis, *GlobalVis;
extern int proclevel;
extern char options[];

/* STATICALLOCDEF "scope" 10 */

/* STATICALLOCDEF "scopelist" 10 */

static int	sc_count;

void open_scope(int scopetype)
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
	if (! sc->sc_scopeclosed) {
		ls->sc_next = ls->sc_encl;
	}
	ls->sc_count = sc_count++;
	CurrVis = ls;
}

t_scope * open_and_close_scope(int scopetype)
{
	t_scope *sc;

	open_scope(scopetype);
	sc = CurrentScope;
	close_scope(0);
	return sc;
}

void InitScope(void)
{
	register t_scope *sc = new_scope();
	register t_scopelist *ls = new_scopelist();

	sc->sc_level = proclevel;
	PervasiveScope = sc;
	ls->sc_scope = PervasiveScope;
	CurrVis = ls;
}

static void chk_proc(register t_def *df)
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

static void chk_forw(t_def **pdf)
{
	/*	Called at scope close. Look for all forward definitions and
		if the scope was a closed scope, give an error message for
		them, and otherwise move them to the enclosing scope.
	*/
	register t_def *df;

	while ( (df = *pdf) ) {
		if (df->df_kind == D_FORWTYPE) {
			pdf = &df->df_nextinscope;
			ForceForwardTypeDef(df);	/* removes df */
			continue;
		}
		if (df->df_kind & (D_FORWARD|D_FORWMODULE)) {
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
			}
			else {
				/* This scope was an open scope.
				   Maybe the definitions are in the
				   enclosing scope?
				*/
				register t_scopelist *ls =
						nextvisible(CurrVis);
				register t_def *df1 = lookup(df->df_idf, ls->sc_scope, 0, 0);

				*pdf = df->df_nextinscope;
	
				if (! df1) {
					if (df->df_kind == D_FORWMODULE) {
						df->for_vis->sc_next = ls;
					}
					df->df_nextinscope = ls->sc_scope->sc_def;
					ls->sc_scope->sc_def = df;
					df->df_scope = ls->sc_scope;
					continue;
				}
				/* leave it like this ??? */
			}
			FreeNode(df->for_node);
		}
		pdf = &df->df_nextinscope;
	}
}

void Reverse(t_def **pdf)
{
	/*	Reverse the order in the list of definitions in a scope.
		This is neccesary because this list is built in reverse.
		Also, while we're at it, remove uninteresting definitions
		from this list.
	*/
	register t_def *df, *df1;
#define INTERESTING (D_MODULE|D_PROCEDURE|D_PROCHEAD|D_VARIABLE|D_IMPORTED|D_TYPE|D_CONST|D_FIELD)

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

void close_scope(int flag)
{
	/*	Close a scope. If "flag" is set, check for forward declarations,
		either POINTER declarations, or EXPORTs, or forward references
		to MODULES
	*/
	register t_scope *sc = CurrentScope;

	assert(sc != 0);

	FreeNode(sc->sc_end);
	sc->sc_end = dot2leaf(Link);

	if (flag) {
		DO_DEBUG(options['S'],(print("List of definitions in currently ended scope:\n"), DumpScope(sc->sc_def)));
		if (flag & SC_CHKPROC) chk_proc(sc->sc_def);
		if (flag & SC_CHKFORW) chk_forw(&(sc->sc_def));
		if (flag & SC_REVERSE) Reverse(&(sc->sc_def));
	}
	CurrVis = enclosing(CurrVis);
}

#ifdef DEBUG
void DumpScope(register t_def *df)
{
	while (df) {
		PrDef(df);
		df = df->df_nextinscope;
	}
}
#endif
