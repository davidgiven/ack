/* D E F I N I T I O N   M E C H A N I S M */

static char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	"Lpars.h"
#include	"def.h"
#include	"idf.h"
#include	"main.h"
#include	"scope.h"
#include	"debug.h"

struct def *h_def;		/* Pointer to free list of def structures */

static struct def illegal_def =
	{0, 0, -20 /* Illegal scope */, D_ERROR};

struct def *ill_df = &illegal_def;

struct def *
define(id, scope, kind)
	register struct idf *id;
{
	/*	Declare an identifier in a scope, but first check if it
		already has been defined. If so, error message.
	*/
	register struct def *df;

	DO_DEBUG(debug(4,"Defining identifier %s in scope %d", id->id_text, scope));
	df = lookup(id, scope);
	if (	/* Already in this scope */
		df
	   ||	/* A closed scope, and id defined in the pervasive scope */
		( CurrentScope == scope 
		&&
		  scopeclosed(currscope)
		&&
		  (df = lookup(id, 0)))
	   ) {
		switch(df->df_kind) {
		case D_PROCHEAD:
			if (kind == D_PROCEDURE) {
				df->df_kind = D_PROCEDURE;
				return df;
			}
			break;	
		case D_HIDDEN:
			if (kind == D_TYPE && state == IMPLEMENTATION) {
				df->df_kind = D_HTYPE;
				return df;
			}
			break;
		case D_ERROR:
		case D_ISEXPORTED:
			df->df_kind = kind;
			return df;
		}
		error("identifier \"%s\" already declared", id->id_text);
		return df;
	}
	df = new_def();
	df->df_idf = id;
	df->df_scope = scope;
	df->df_kind = kind;
	df->next = id->id_def;
	id->id_def = df;
	return df;
}

struct def *
lookup(id, scope)
	register struct idf *id;
{
	/*	Look up a definition of an identifier in scope "scope".
		Make the "def" list self-organizing.
		Return a pointer to its "def" structure if it exists,
		otherwise return 0.
	*/
	register struct def *df, *df1;

	df1 = 0;
	df = id->id_def;
	DO_DEBUG(debug(4,"Looking for identifier %s in scope %d", id->id_text, scope));
	while (df) {
		if (df->df_scope == scope) {
			if (df1) {
				df1->next = df->next;
				df->next = id->id_def;
				id->id_def = df;
			}
			return df;
		}
		df1 = df;
		df = df->next;
	}
	return 0;
}
