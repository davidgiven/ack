/* D E F I N I T I O N   M E C H A N I S M */

static char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>
#include	"main.h"
#include	"def.h"
#include	"type.h"
#include	"idf.h"
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
	register struct scope *scope;
{
	/*	Declare an identifier in a scope, but first check if it
		already has been defined. If so, error message.
	*/
	register struct def *df;

	DO_DEBUG(5, debug("Defining identifier \"%s\" in scope %d, kind = %d",
			  id->id_text, scope->sc_scope, kind));
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
				/* Definition of which the heading was
				   already seen in a definition module
				*/
				df->df_kind = kind;
				return df;
			}
			break;	
		case D_HIDDEN:
			if (kind == D_TYPE && !DefinitionModule) {
				df->df_kind = D_HTYPE;
				return df;
			}
			break;
		case D_FORWMODULE:
			if (kind & (D_FORWMODULE|D_MODULE)) {
				df->df_kind = kind;
				return df;
			}
			break;
		case D_ERROR:
		case D_FORWARD:
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
	df->df_flags = 0;
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

			if (df->df_kind == D_UNDEF_IMPORT) {	
				df1 = df->imp_def;
				assert(df1 != 0);
				if (df1->df_kind == D_MODULE) {
					df1 = lookup(id, df1->mod_scope);
					if (df1) {
						df->df_kind = D_IMPORT;
						df->imp_def = df1;
					}
					return df1;
				}
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
	register struct def *df, *df1;

	while (ids) {
		df = define(ids->nd_IDF, CurrentScope, D_FORWARD);
		if (qualified) {
			df->df_flags |= D_QEXPORTED;
		}
		else {
			df->df_flags |= D_EXPORTED;
			df1 = lookup(ids->nd_IDF,
				     enclosing(CurrentScope)->sc_scope);
			if (! df1 || !(df1->df_kind & (D_PROCHEAD|D_HIDDEN))) {
				df1 = define(ids->nd_IDF,
						enclosing(CurrentScope),
						D_IMPORT);
			}
			else {
				/* A hidden type or a procedure of which only
				   the head is seen. Apparently, they are
				   exported from a local module!
				*/
				df->df_kind = df1->df_kind;
				df1->df_kind = D_IMPORT;
			}
			df1->imp_def = df;
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
	int scope;
	int kind;
	int imp_kind;
#define FROM_MODULE	0
#define FROM_ENCLOSING	1
	struct def *lookfor(), *GetDefinitionModule();

	kind = D_IMPORT;
	scope = enclosing(CurrentScope)->sc_scope;
	if (!idn) imp_kind = FROM_ENCLOSING;
	else {
		imp_kind = FROM_MODULE;
		if (local) {
			df = lookfor(idn, enclosing(CurrentScope), 0);
			if (df->df_kind == D_ERROR) {
				/* The module from which the import was done
				   is not yet declared. I'm not sure if I must
				   accept this, but for the time being I will.
				   ???
				*/
				df->df_scope = scope;
				df->df_kind = D_FORWMODULE;
				df->mod_scope = -1;
				kind = D_UNDEF_IMPORT;
			}
		}
		else {
			df = GetDefinitionModule(idn->nd_IDF);
		}
		if (!(df->df_kind & (D_MODULE|D_FORWMODULE))) {
			/* enter all "ids" with type D_ERROR */
			kind = D_ERROR;
			if (df->df_kind != D_ERROR) {
node_error(idn, "identifier \"%s\" does not represent a module", idn->nd_IDF->id_text);
			}
		}
		else	scope = df->mod_scope;
	}
	while (ids) {
		if (imp_kind == FROM_MODULE) {
			if (scope == -1) {
			}
			else if (!(df = lookup(ids->nd_IDF, scope))) {
node_error(ids, "identifier \"%s\" not declared in qualifying module",
ids->nd_IDF->id_text);
				df = ill_df;
			}
			else if (!(df->df_flags&(D_EXPORTED|D_QEXPORTED))) {
node_error(ids,"identifier \"%s\" not exported from qualifying module",
ids->nd_IDF->id_text);
			}
		}
		else {
			if (local) {
				df = lookfor(ids, enclosing(CurrentScope), 0);
			} else df = GetDefinitionModule(ids->nd_IDF);
			if (df->df_kind == D_ERROR) {
node_error(ids, "identifier \"%s\" not visible in enclosing scope",
ids->nd_IDF->id_text);
			}
		}
		DO_DEBUG(2, debug("importing \"%s\", kind %d", ids->nd_IDF->id_text, df->df_kind));
		define(ids->nd_IDF, CurrentScope, kind)->imp_def = df;
		if (df->df_kind == D_TYPE &&
		    df->df_type->tp_fund == T_ENUMERATION) {
			/* Also import all enumeration literals */
			exprt_literals(df->df_type->enm_enums,
					CurrentScope);
		}
		ids = ids->next;
	}
}

exprt_literals(df, toscope)
	register struct def *df;
	struct scope *toscope;
{
	/*	A list of enumeration literals is exported. This is implemented
		as an import from the scope "toscope".
	*/
	DO_DEBUG(2, debug("enumeration import:"));
	while (df) {
		DO_DEBUG(2, debug(df->df_idf->id_text));
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
