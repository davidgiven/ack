/* D E F I N I T I O N   M E C H A N I S M */

static char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>
#include	"Lpars.h"
#include	"def.h"
#include	"idf.h"
#include	"misc.h"
#include	"main.h"
#include	"scope.h"
#include	"debug.h"

struct def *h_def;		/* Pointer to free list of def structures */

static struct def illegal_def =
	{0, 0, 0, -20 /* Illegal scope */, D_ERROR};

struct def *ill_df = &illegal_def;

struct def *
define(id, scope, kind)
	register struct idf *id;
{
	/*	Declare an identifier in a scope, but first check if it
		already has been defined. If so, error message.
	*/
	register struct def *df;
	register struct scope *sc;

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

	/* enter the definition in the list of definitions in this scope */
	sc = currscope;
	while (sc->sc_scope != scope) {
		sc = sc->next;
		assert(sc != 0);
	}
	df->df_nextinscope = sc->sc_def;
	sc->sc_def = df;
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
			if (df->df_kind == D_IMPORT) {
				df = lookup(id, df->imp_scopenr);
				assert(df != 0);
				return df;
				/* ??? But this does damage to the self-
				   organizing character of the list
				*/
			}
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

/*	From the current scope, the list of identifiers "ids" is
	exported. Note this fact. If the export is not qualified, make
	all the "ids" visible in the enclosing scope by defining them
	in this scope as "imported".
*/
Export(ids, qualified)
	register struct id_list *ids;
{
	register struct def *df;

	while (ids) {
		df = define(ids->id_ptr, CurrentScope, D_ISEXPORTED);
		if (qualified) {
			df->df_flags |= D_QEXPORTED;
		}
		else {
			df->df_flags |= D_EXPORTED;
			df = define(ids->id_ptr, enclosing(currscope)->sc_scope,
					D_IMPORT);
		}
		ids = ids->next;
	}
}

/*	"ids" is a list of imported identifiers.
	If "id" is a null-pointer, the identifiers are imported from the
	enclosing scope. Otherwise they are imported from the module
	indicated by "id", ehich must be visible in the enclosing scope.
	An exception must be made for imports of the Compilation Unit.
	This case is indicated by  the value 0 of the flag "local".
	In this case, if "id" is a null pointer, the "ids" identifiers
	are all module identifiers. Their Definition Modules must be read.
	Otherwise "id" is a module identifier whose Definition Module must
	be read. "ids" then represents a list of identifiers defined in
	this module.
*/
Import(ids, id, local)
	register struct id_list *ids;
	struct idf *id;
{
	register struct def *df;
	int scope;
	int kind;
	struct def *lookfor();

	if (local) {
		kind = D_IMPORT;
		if (!id) scope = enclosing(currscope)->sc_scope;
		else {
			df = lookfor(id, 1);
			if (df->df_kind != D_MODULE) {
				if (df->df_kind != D_ERROR) {
error("identifier \"%s\" does not represent a module", id->id_text);
				}
				/* enter all "ids" with type D_ERROR */
				kind = D_ERROR;
				scope = enclosing(currscope)->sc_scope;
			}
			else	scope = df->mod_scope;
		}
		while (ids) {
			df = lookup(ids->id_ptr, scope);
			if (!df) {
				error("identifier \"%s\" not declared",
				      ids->id_ptr->id_text);
			}
			df = define(ids->id_ptr, CurrentScope, D_IMPORT);
			df->imp_scopenr = scope;
			ids = ids->next;
		}
		return;
	}
	/* ???? */
}
