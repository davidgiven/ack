/* L O O K U P   R O U T I N E S */

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>

#include	"LLlex.h"
#include	"def.h"
#include	"idf.h"
#include	"misc.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"

remove_def(df)
	register struct def *df;
{
	struct idf *id= df->df_idf;
	struct def *df1 = id->id_def;

	if( df1 == df ) id->id_def = df->df_next;
	else {
		while( df1 && df1->df_next != df ) df1 = df1->df_next;
		df1->df_next = df->df_next;
	}
	free_def(df);
}

struct def *
lookup(id, scope, inuse)
	register struct idf *id;
	struct scope *scope;
	long	inuse;
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
	for( df = id->id_def, df1 = 0;
	     df && df->df_scope != scope;
	     df1 = df, df = df->df_next ) { /* nothing */ }

	if( df )	{
		/* Found it
		*/
		if( df1) {
			/* Put the definition in front
			*/
			df1->df_next = df->df_next;
			df->df_next = id->id_def;
			id->id_def = df;
		}
		while( df->df_kind & inuse ) {
			assert(df->usd_def != 0);
			df=df->usd_def;
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
		If it is not defined create a dummy definition and
		if give_error is set, give an error message.
	*/
	register struct def *df, *tmp_df;
	register struct scopelist *sc = vis;

	while( sc )	{
		df = lookup(id->nd_IDF, sc->sc_scope, D_INUSE);
		if( df ) {
			while( vis->sc_scope->sc_level >
				sc->sc_scope->sc_level ) {
				if( tmp_df = define(id->nd_IDF, vis->sc_scope,
					D_INUSE))
					tmp_df->usd_def = df;
			    vis = nextvisible(vis);
			}
		/* Since the scope-level of standard procedures is the
		 * same as for the user-defined procedures, the procedure
		 * must be marked as used. Not doing so would mean that
		 * such a procedure could redefined after usage.
		 */
			if( (vis->sc_scope == GlobalScope) &&
			    !lookup(id->nd_IDF, GlobalScope, D_INUSE) ) { 
				if( tmp_df = define(id->nd_IDF, vis->sc_scope,
					D_INUSE))
					tmp_df->usd_def = df;
			}

			return df;
		}
		sc = nextvisible(sc);
	}

	if( give_error ) id_not_declared(id);

	df = MkDef(id->nd_IDF, vis->sc_scope, D_ERROR);
	return df;
}
