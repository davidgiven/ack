/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* L O O K U P   R O U T I N E S */

/* $Header$ */

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>

#include	"LLlex.h"
#include	"def.h"
#include	"idf.h"
#include	"scope.h"
#include	"node.h"
#include	"type.h"
#include	"misc.h"

extern int	pass_1;

t_def *
lookup(id, scope, import, flags)
	register t_idf *id;
	t_scope *scope;
{
	/*	Look up a definition of an identifier in scope "scope".
		Make the "def" list self-organizing.
		Return a pointer to its "def" structure if it exists,
		otherwise return 0.
	*/
	register t_def *df, *df1;

	/* Look in the chain of definitions of this "id" for one with scope
	   "scope".
	*/
	for (df = id->id_def, df1 = 0;
	     df && df->df_scope != scope;
	     df1 = df, df = df->df_next) { /* nothing */ }

	if (! df && import && scopeclosed(scope)) {
		for (df = id->id_def, df1 = 0;
		     df && df->df_scope != PervasiveScope;
		     df1 = df, df = df->df_next) { /* nothing */ }
	}

	if (df) {
		/* Found it
		*/
		if (df1) {
			/* Put the definition in front
			*/
			df1->df_next = df->df_next;
			df->df_next = id->id_def;
			id->id_def = df;
		}
		df->df_flags |= flags;
		while (df->df_kind & import) {
			assert(df->imp_def != 0);
			df = df->imp_def;
		}
	}
	return df;
}

t_def *
lookfor(id, vis, message, flags)
	register t_node *id;
	register t_scopelist *vis;
{
	/*	Look for an identifier in the visibility range started by "vis".
		If it is not defined create a dummy definition and,
		if message is set, give an error message
	*/
	register t_scopelist *sc = vis;
	t_def *df;

	while (sc) {
		df = lookup(id->nd_IDF, sc->sc_scope, D_IMPORTED, flags);
		if (df) {
			if (pass_1 && message) {
				while (vis->sc_scope->sc_level >
				       sc->sc_scope->sc_level) {
					define( id->nd_IDF,
						vis->sc_scope,
						D_INUSE)-> imp_def = df;
					vis = nextvisible(vis);
				}
			}
			return df;
		}
		sc = nextvisible(sc);
	}

	if (message) id_not_declared(id);

	return MkDef(id->nd_IDF, vis->sc_scope, D_ERROR);
}
