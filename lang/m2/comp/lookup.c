/* L O O K U P   R O U T I N E S */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>

#include	"def.h"
#include	"idf.h"
#include	"scope.h"
#include	"LLlex.h"
#include	"node.h"

extern struct def	*MkDef();

struct def *
lookup(id, scope)
	register struct idf *id;
	struct scope *scope;
{
	/*	Look up a definition of an identifier in scope "scope".
		Make the "def" list self-organizing.
		Return a pointer to its "def" structure if it exists,
		otherwise return 0.
	*/
	register struct def *df;
	struct def *df1;

	for (df = id->id_def, df1 = 0; df; df1 = df, df = df->next) {
		if (df->df_scope == scope) {
			if (df1) {
				/* Put the definition in front
				*/
				df1->next = df->next;
				df->next = id->id_def;
				id->id_def = df;
			}
			if (df->df_kind == D_IMPORT) {
				assert(df->imp_def != 0);
				return df->imp_def;
			}
			return df;
		}
	}
	return 0;
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
	struct def *df;
	register struct scopelist *sc = vis;

	while (sc) {
		df = lookup(id->nd_IDF, sc->sc_scope);
		if (df) return df;
		sc = nextvisible(sc);
	}

	if (give_error) id_not_declared(id);

	return MkDef(id->nd_IDF, vis->sc_scope, D_ERROR);
}
