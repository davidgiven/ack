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
	{0, 0, 0, 0, D_ERROR};

struct def *ill_df = &illegal_def;

struct def *
MkDef(id, scope, kind)
	struct idf *id;
	struct scope *scope;
{
	/*	Create a new definition structure in scope "scope", with
		id "id" and kind "kind".
	*/
	register struct def *df;

	df = new_def();
	df->df_flags = 0;
	df->df_idf = id;
	df->df_scope = scope;
	df->df_kind = kind;
	df->df_type = 0;
	df->next = id->id_def;
	id->id_def = df;

	/* enter the definition in the list of definitions in this scope
	*/
	df->df_nextinscope = scope->sc_def;
	scope->sc_def = df;
	return df;
}

struct def *
define(id, scope, kind)
	register struct idf *id;
	register struct scope *scope;
{
	/*	Declare an identifier in a scope, but first check if it
		already has been defined. If so, error message.
	*/
	register struct def *df;

	DO_DEBUG(5, debug("Defining identifier \"%s\", kind = %d",
			  id->id_text, kind));
	df = lookup(id, scope);
	if (	/* Already in this scope */
		df
	   ||	/* A closed scope, and id defined in the pervasive scope */
		( CurrentScope == scope 
		&&
		  scopeclosed(scope)
		&&
		  (df = lookup(id, PervasiveScope)))
	   ) {
		switch(df->df_kind) {
		case D_HIDDEN:
			if (kind == D_TYPE && !DefinitionModule) {
				df->df_kind = D_HTYPE;
				return df;
			}
			break;
		case D_FORWMODULE:
			if (kind == D_FORWMODULE) {
				return df;
			}
			if (kind == D_MODULE) {
				FreeNode(df->for_node);
				df->mod_scope = df->for_scope;
				df->df_kind = kind;
				return df;
			}
			break;
		case D_FORWARD:
			if (kind != D_FORWARD) {
				FreeNode(df->for_node);
			}
			/* Fall Through */
		case D_ERROR:
			df->df_kind = kind;
			return df;
		}
		if (kind != D_ERROR) {
error("identifier \"%s\" already declared", id->id_text);
		}
		return df;
	}
	return MkDef(id, scope, kind);
}

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
	struct def *retval;

	df1 = 0;
	df = id->id_def;
	while (df) {
		if (df->df_scope == scope) {
			retval = df;
			if (df->df_kind == D_IMPORT) {
				retval = df->imp_def;
				assert(retval != 0);
			}

			if (df1) {
				df1->next = df->next;
				df->next = id->id_def;
				id->id_def = df;
			}
			return retval;
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
	struct node *nd = ids;

	while (ids) {
		df = lookup(ids->nd_IDF, CurrentScope);
		if (df && (df->df_flags & (D_EXPORTED|D_QEXPORTED))) {
node_error(ids, "Identifier \"%s\" occurs more than once in export list",
df->df_idf->id_text);
		}
		else if (!df) {
			df = define(ids->nd_IDF, CurrentScope, D_FORWARD);
			df->for_node = MkNode(Name,NULLNODE,NULLNODE,
					&(ids->nd_token));
		}
		if (qualified) {
			df->df_flags |= D_QEXPORTED;
		}
		else {
			df->df_flags |= D_EXPORTED;
			df1 = lookup(ids->nd_IDF, enclosing(CurrentScope));
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
				df->df_value.df_forward = df1->df_value.df_forward;
				df1->df_kind = D_IMPORT;
			}
			df1->imp_def = df;
		}
		ids = ids->next;
	}
	FreeNode(nd);
}

static struct scope *
ForwModule(df, idn)
	register struct def *df;
	struct node *idn;
{
	/*	An import is done from a not yet defined module "idn".
		Create a declaration and a scope for this module.
	*/
	struct scope *scope;

	df->df_scope = enclosing(CurrentScope);
	df->df_kind = D_FORWMODULE;
	open_scope(CLOSEDSCOPE);
	scope = CurrentScope;	/* The new scope, but watch out, it's "next"
				   field is not set right. It must indicate the
				   enclosing scope, but this must be done AFTER
				   closing this one
				*/
	df->for_scope = scope;
	df->for_node = MkNode(Name, NULLNODE, NULLNODE, &(idn->nd_token));
	close_scope(0);	
	scope->next = df->df_scope;
				/* Here ! */
	return scope;
}

static struct def *
ForwDef(ids, scope)
	register struct node *ids;
	struct scope *scope;
{
	/*	Enter a forward definition of "ids" in scope "scope",
		if it is not already defined.
	*/
	register struct def *df;

	if (!(df = lookup(ids->nd_IDF, scope))) {
		df = define(ids->nd_IDF, scope, D_FORWARD);
		df->for_node = MkNode(Name,NULLNODE,NULLNODE,&(ids->nd_token));
	}
	return df;
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
	struct scope *scope = enclosing(CurrentScope);
	int kind = D_IMPORT;
	int forwflag = 0;
#define FROM_MODULE	0
#define FROM_ENCLOSING	1
	int imp_kind = FROM_ENCLOSING;
	struct def *lookfor(), *GetDefinitionModule();

	if (idn) {
		imp_kind = FROM_MODULE;
		if (local) {
			df = lookfor(idn, scope, 0);
			switch(df->df_kind) {
			case D_ERROR:
				/* The module from which the import was done
				   is not yet declared. I'm not sure if I must
				   accept this, but for the time being I will.
				   ???
				*/
				scope = ForwModule(df, idn);
				forwflag = 1;
				break;
			case D_FORWMODULE:
				scope = df->for_scope;
				break;
			case D_MODULE:
				scope = df->mod_scope;
				break;
			default:
				kind = D_ERROR;
node_error(idn, "identifier \"%s\" does not represent a module",
idn->nd_IDF->id_text);
				break;
			}
		}
		else	scope = GetDefinitionModule(idn->nd_IDF)->mod_scope;

		FreeNode(idn);
	}

	idn = ids;
	while (ids) {
		if (imp_kind == FROM_MODULE) {
			if (forwflag) {
				df = ForwDef(ids, scope);
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
			if (local) df = ForwDef(ids, scope);
			else	df = GetDefinitionModule(ids->nd_IDF);
		}

DO_DEBUG(2, debug("importing \"%s\", kind %d", ids->nd_IDF->id_text,
df->df_kind));
		define(ids->nd_IDF, CurrentScope, kind)->imp_def = df;
		if (df->df_kind == D_TYPE &&
		    df->df_type->tp_fund == T_ENUMERATION) {
			/* Also import all enumeration literals
			*/
			exprt_literals(df->df_type->enm_enums, CurrentScope);
		}
		ids = ids->next;
	}

	FreeNode(idn);
}

exprt_literals(df, toscope)
	register struct def *df;
	struct scope *toscope;
{
	/*	A list of enumeration literals is exported. This is implemented
		as an import from the scope "toscope".
	*/
	DO_DEBUG(3, debug("enumeration import:"));
	while (df) {
		DO_DEBUG(3, debug(df->df_idf->id_text));
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
	register struct def *df = *pdf;

	while (df) {
		if (df->df_kind == D_IMPORT) {
			RemFromId(df);
			*pdf = df->df_nextinscope;
			free_def(df);
		}
		else {
			pdf = &(df->df_nextinscope);
		}
		df = *pdf;
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

struct def *
DeclProc(type)
{
	/*	A procedure is declared, either in a definition or a program
		module. Create a def structure for it (if neccessary)
	*/
	register struct def *df;
	extern char *sprint(), *Malloc(), *strcpy();
	static int nmcount = 0;
	char buf[256];

	assert(type & (D_PROCEDURE | D_PROCHEAD));

	if (type == D_PROCHEAD) {
		/* In a definition module
		*/
		df = define(dot.TOK_IDF, CurrentScope, type);
		df->for_node = MkNode(Name, NULLNODE, NULLNODE, &dot);
		sprint(buf,"%s_%s",CurrentScope->sc_name,df->df_idf->id_text);
		df->for_name = Malloc((unsigned) (strlen(buf)+1));
		strcpy(df->for_name, buf);
		C_exp(df->for_name);
	}
	else {
		df = lookup(dot.TOK_IDF, CurrentScope);
		if (df && df->df_kind == D_PROCHEAD) {
			/* C_exp already generated when we saw the definition
			   in the definition module
			*/
			df->df_kind = D_PROCEDURE;
			open_scope(OPENSCOPE);
			CurrentScope->sc_name = df->for_name;
			df->prc_scope = CurrentScope;
		}
		else {
			df = define(dot.TOK_IDF, CurrentScope, type);
			if (CurrentScope != Defined->mod_scope) {
				sprint(buf, "_%d_%s", ++nmcount,
					df->df_idf->id_text);
			}
			else	(sprint(buf, "%s_%s",df->df_scope->sc_name,
						df->df_idf->id_text));
			open_scope(OPENSCOPE);
			df->prc_scope = CurrentScope;
			CurrentScope->sc_name = Malloc((unsigned)(strlen(buf)+1));
			strcpy(CurrentScope->sc_name, buf);
			C_inp(buf);
		}
		df->prc_nbpar = 0;
	}

	return df;
}

InitProc(nd, df)
	struct node *nd;
	struct def *df;
{
	/*	Create an initialization procedure for a module.
	*/
	df->mod_body = nd;
	/* Keep it this way, or really create a procedure out of it??? */
}

#ifdef DEBUG
PrDef(df)
	register struct def *df;
{
	debug("name: %s, kind: %d", df->df_idf->id_text, df->df_kind);
}
#endif DEBUG
