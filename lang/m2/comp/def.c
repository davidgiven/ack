/* D E F I N I T I O N   M E C H A N I S M */

static char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>
#include	"Lpars.h"
#include	"def.h"
#include	"type.h"
#include	"idf.h"
#include	"main.h"
#include	"scope.h"
#include	"LLlex.h"
#include	"node.h"
#include	"debug.h"

struct def *h_def;		/* Pointer to free list of def structures */

static struct def illegal_def =
	{0, 0, 0, -20 /* Illegal scope */, D_ERROR};

struct def *ill_df = &illegal_def;

struct def *
define(id, scope, kind)
	register struct idf *id;
	struct scope *scope;
{
	/*	Declare an identifier in a scope, but first check if it
		already has been defined. If so, error message.
	*/
	register struct def *df;
	register struct scope *sc;

	DO_DEBUG(5, debug("Defining identifier \"%s\" in scope %d", id->id_text, scope->sc_scope));
	df = lookup(id, scope->sc_scope);
	if (	/* Already in this scope */
		df
	   ||	/* A closed scope, and id defined in the pervasive scope */
		( CurrentScope == scope 
		&&
		  scopeclosed(CurrentScope)
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
			if (kind == D_TYPE && !DefinitionModule) {
				df->df_kind = D_HTYPE;
				return df;
			}
			break;
		case D_ERROR:
		case D_ISEXPORTED:
			df->df_kind = kind;
			return df;
		}
		if (kind != D_ERROR) {
error("identifier \"%s\" already declared", id->id_text);
		}
		return df;
	}
	df = new_def();
	df->df_idf = id;
	df->df_scope = scope->sc_scope;
	df->df_kind = kind;
	df->next = id->id_def;
	id->id_def = df;

	/* enter the definition in the list of definitions in this scope */
	df->df_nextinscope = scope->sc_def;
	scope->sc_def = df;
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
	DO_DEBUG(5, debug("Looking for identifier \"%s\" in scope %d", id->id_text, scope));
	while (df) {
		if (df->df_scope == scope) {
			if (df->df_kind == D_IMPORT) {
				df = df->imp_def;
				assert(df != 0);
				return df;
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

Export(ids, qualified)
	register struct node *ids;
{
	/*	From the current scope, the list of identifiers "ids" is
		exported. Note this fact. If the export is not qualified, make
		all the "ids" visible in the enclosing scope by defining them
		in this scope as "imported".
	*/
	register struct def *df;

	while (ids) {
		df = define(ids->nd_IDF, CurrentScope, D_ISEXPORTED);
		if (qualified) {
			df->df_flags |= D_QEXPORTED;
		}
		else {
			df->df_flags |= D_EXPORTED;
			df = define(ids->nd_IDF, enclosing(CurrentScope),
					D_IMPORT);
		}
		ids = ids->next;
	}
}

Import(ids, idn, local)
	register struct node *ids;
	struct node *idn;
{
	/*	"ids" is a list of imported identifiers.
		If "idn" is a null-pointer, the identifiers are imported from
		the enclosing scope. Otherwise they are imported from the module
		indicated by "idn", which must be visible in the enclosing
		scope.  An exception must be made for imports of the
		Compilation Unit.
		This case is indicated by  the value 0 of the flag "local".
		In this case, if "idn" is a null pointer, the "ids" identifiers
		are all module identifiers. Their Definition Modules must be
		read.  Otherwise "idn" is a module identifier whose Definition
		Module must be read. "ids" then represents a list of
		identifiers defined in this module.
	*/
	register struct def *df;
	register struct idf *id = 0;
	int scope;
	int kind;
	int imp_kind;
#define FROM_MODULE	0
#define FROM_ENCLOSING	1
	struct def *lookfor(), *GetDefinitionModule();

	if (idn) id = idn->nd_IDF;
	kind = D_IMPORT;
	scope = enclosing(CurrentScope)->sc_scope;
	if (!id) imp_kind = FROM_ENCLOSING;
	else {
		imp_kind = FROM_MODULE;
		if (local) df = lookfor(id, enclosing(CurrentScope), 1);
		else df = GetDefinitionModule(id);
		if (df->df_kind != D_MODULE) {
			/* enter all "ids" with type D_ERROR */
			kind = D_ERROR;
			if (df->df_kind != D_ERROR) {
node_error(idn, "identifier \"%s\" does not represent a module", id->id_text);
			}
		}
		else	scope = df->mod_scope;
	}
	while (ids) {
		if (imp_kind == FROM_MODULE) {
			if (!(df = lookup(ids->nd_IDF, scope))) {
node_error(ids, "identifier \"%s\" not declared in qualifying module",
ids->nd_IDF->id_text);
				df = ill_df;
			}
			else 
			if (!(df->df_flags&(D_EXPORTED|D_QEXPORTED))) {
node_error(ids,"identifier \"%s\" not exported from qualifying module",
ids->nd_IDF->id_text);
			}
		}
		else {
			if (local) {
				df = lookfor(ids->nd_IDF,
					     enclosing(CurrentScope), 0);
			} else df = GetDefinitionModule(ids->nd_IDF);
			if (df->df_kind == D_ERROR) {
node_error(ids, "identifier \"%s\" not visible in enclosing scope",
ids->nd_IDF->id_text);
			}
		}
		define(ids->nd_IDF, CurrentScope, kind)->imp_def = df;
		if (df->df_kind == D_TYPE &&
		    df->df_type->tp_fund == ENUMERATION) {
			/* Also import all enumeration literals */
			exprt_literals(df->df_type->enm_enums,
					CurrentScope);
		}
		ids = ids->next;
	}
}

exprt_literals(df, toscope)
	register struct def *df;
	register struct scope *toscope;
{
	/*	A list of enumeration literals is exported. This is implemented
		as an import from the scope "toscope".
	*/
	while (df) {
		define(df->df_idf, toscope, D_IMPORT)->imp_def = df;
		df = df->enm_next;
	}
}

RemImports(pdf)
	struct def **pdf;
{
	/*	Remove all imports from a definition module. This is
		neccesary because the implementation module might import
		them again.
	*/
	register struct def *df = *pdf, *df1 = 0;

	while (df) {
		if (df->df_kind == D_IMPORT) {
			RemFromId(df);
			if (df1) {
				df1->df_nextinscope = df->df_nextinscope;
				free_def(df);
				df = df1->df_nextinscope;
			}
			else {
				*pdf = df->df_nextinscope;
				free_def(df);
				df = *pdf;
			}
		}
		else {
			df1 = df;
			df = df->df_nextinscope;
		}
	}
}

RemFromId(df)
	struct def *df;
{
	/*	Remove definition "df" from the definition list
	*/
	register struct idf *id = df->df_idf;
	register struct def *df1;

	if (id->id_def == df) id->id_def = df->next;
	else {
		df1 = id->id_def;
		while (df1->next != df) {
			assert(df1->next != 0);
			df1 = df1->next;
		}
		df1->next = df->next;
	}
}
