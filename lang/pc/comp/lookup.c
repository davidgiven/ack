/* L O O K U P   R O U T I N E S */

#include	<em_arith.h>
#include	<em_label.h>

#include	"LLlex.h"
#include	"def.h"
#include	"idf.h"
#include	"misc.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"

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
	register struct def *df, *df1;

	/* Look in the chain of definitions of this "id" for one with scope
	   "scope".
	*/
	for( df = id->id_def, df1 = 0;
	     df && df->df_scope != scope;
	     df1 = df, df = df->df_next ) { /* nothing */ }

	if( df && df1 )	{
		/* Put the definition in front
		*/
		df1->df_next = df->df_next;
		df->df_next = id->id_def;
		id->id_def = df;
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
	register struct def *df;
	register struct scopelist *sc = vis;

	while( sc )	{
		df = lookup(id->nd_IDF, sc->sc_scope);
		if( df ) return df;
		sc = nextvisible(sc);
	}

	if( give_error ) id_not_declared(id);

	df = MkDef(id->nd_IDF, vis->sc_scope, D_ERROR);
	return df;
}
