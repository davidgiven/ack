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

struct def *
lookup(id, scope, import)
	register struct idf *id;
	struct scope *scope;
{
	/*	Look up a definition of an identifier in scope "scope".
		Make the "def" list self-organizing.
		Return a pointer to its "def" structure if it exists,
		otherwise return 0.
	*/
	register struct def *df, *df1;

	/* Look in the chain of definitions of this "id" for one with scope
	   "scope".
	*/
	for (df = id->id_def, df1 = 0;
	     df && df->df_scope != scope;
	     df1 = df, df = df->df_next) { /* nothing */ }

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
		if (import) {
			while (df->df_kind == D_IMPORT) {
				assert(df->imp_def != 0);
				df = df->imp_def;
			}
		}
	}
	return df;
}

struct def *
lookfor(id, vis, give_error)
	register struct node *id;
	struct scopelist *vis;
{
	/*	Look for an identifier in the visibility range started by "vis".
		If it is not defined create a dummy definition and,
		if "give_error" is set, give an error message.
	*/
	register struct def *df;
	register struct scopelist *sc = vis;

	while (sc) {
		df = lookup(id->nd_IDF, sc->sc_scope, 1);
		if (df) return df;
		sc = nextvisible(sc);
	}

	if (give_error) id_not_declared(id);

	df = MkDef(id->nd_IDF, vis->sc_scope, D_ERROR);
	df->df_type = error_type;
	return df;
}
