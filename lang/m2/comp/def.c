/* D E F I N I T I O N   M E C H A N I S M */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

#include	"debug.h"

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
#include	"Lpars.h"

struct def *h_def;		/* pointer to free list of def structures */
#ifdef DEBUG
int	cnt_def;		/* count number of allocated ones */
#endif

struct def *ill_df;

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
	clear((char *) df, sizeof (*df));
	df->df_idf = id;
	df->df_scope = scope;
	df->df_kind = kind;
	df->next = id->id_def;
	id->id_def = df;

	/* enter the definition in the list of definitions in this scope
	*/
	df->df_nextinscope = scope->sc_def;
	scope->sc_def = df;
	return df;
}

InitDef()
{
	/*	Initialize this module. Easy, the only thing to be initialized
		is "illegal_def".
	*/
	struct idf *gen_anon_idf();

	ill_df = MkDef(gen_anon_idf(), CurrentScope, D_ERROR);
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

	df = lookup(id, scope);
	if (	/* Already in this scope */
		df
	   ||	/* A closed scope, and id defined in the pervasive scope */
		( 
		  scopeclosed(scope)
		&&
		  (df = lookup(id, PervasiveScope)))
	   ) {
		switch(df->df_kind) {
		case D_HIDDEN:
			if (kind == D_TYPE && !DefinitionModule) {
				df->df_kind = D_TYPE;
				return df;
			}
			break;

		case D_FORWMODULE:
			if (kind == D_FORWMODULE) {
				return df;
			}

			if (kind == D_MODULE) {
				FreeNode(df->for_node);
				df->mod_vis = df->for_vis;
				df->df_kind = kind;
				DefInFront(df);
				return df;
			}
			break;

		case D_FORWARD:
			if (kind != D_FORWARD) {
				FreeNode(df->for_node);
			}

			df->df_kind = kind;
			return df;

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

DoImport(df, scope)
	struct def *df;
	struct scope *scope;
{
	register struct def *df1;

	if (df->df_kind == D_TYPE && df->df_type->tp_fund == T_ENUMERATION) {
		/* Also import all enumeration literals
		*/
		df1 = df->df_type->enm_enums;
		while (df1) {
			define(df1->df_idf, scope, D_IMPORT)->imp_def = df1;
			df1 = df1->enm_next;
		}
	}
	else if (df->df_kind == D_MODULE) {
		/* Also import all definitions that are exported from this
		   module
		*/
		df1 = df->mod_vis->sc_scope->sc_def;
		while (df1) {
			if (df1->df_flags & D_EXPORTED) {
				define(df1->df_idf, scope, D_IMPORT)->imp_def = df1;
			}
			df1 = df1->df_nextinscope;
		}
	}
}

Export(ids, qualified, moddef)
	register struct node *ids;
	struct def *moddef;
{
	/*	From the current scope, the list of identifiers "ids" is
		exported. Note this fact. If the export is not qualified, make
		all the "ids" visible in the enclosing scope by defining them
		in this scope as "imported".
	*/
	register struct def *df, *df1;
	register struct def *impmod;

	for (;ids; ids = ids->next) {
		df = lookup(ids->nd_IDF, CurrentScope);

		if (!df) {
			/* undefined item in export list
			*/
node_error(ids, "identifier \"%s\" not defined", ids->nd_IDF->id_text);
			continue;
		}

		if (df->df_flags & (D_EXPORTED|D_QEXPORTED)) {
node_error(ids, "Identifier \"%s\" occurs more than once in export list",
df->df_idf->id_text);
		}

		if (qualified) {
			df->df_flags |= D_QEXPORTED;
		}
		else {
			/* Export, but not qualified.
			   Find all imports of the module in which this export
			   occurs, and export the current definition to it
			*/
			impmod = moddef->df_idf->id_def;
			while (impmod) {
				if (impmod->df_kind == D_IMPORT &&
				    impmod->imp_def == moddef) {
					DoImport(df, impmod->df_scope);
				}
				impmod = impmod->next;
			}

			df->df_flags |= D_EXPORTED;
			df1 = lookup(ids->nd_IDF, enclosing(CurrVis)->sc_scope);
			if (df1 && df1->df_kind == D_PROCHEAD) {
				if (df->df_kind == D_PROCEDURE) {
					df1->df_kind = D_IMPORT;
					df1->imp_def = df;
					continue;
				}
			}
			else if (df1 && df1->df_kind == D_HIDDEN) {
				if (df->df_kind == D_TYPE) {
					if (df->df_type->tp_fund != T_POINTER) {
error("opaque type \"%s\" is not a pointer type", df->df_idf->id_text);
					}
					df->df_kind = D_TYPE;
					df1->df_kind = D_IMPORT;
					df1->imp_def = df;
					continue;
				}
			}

			df1 = define(ids->nd_IDF,
						enclosing(CurrVis)->sc_scope,
						D_IMPORT);
			df1->imp_def = df;
			DoImport(df, enclosing(CurrVis)->sc_scope);
		}
	}
}

static struct scopelist *
ForwModule(df, idn)
	register struct def *df;
	struct node *idn;
{
	/*	An import is done from a not yet defined module "idn".
		Create a declaration and a scope for this module.
	*/
	struct scopelist *vis;

	df->df_scope = enclosing(CurrVis)->sc_scope;
	df->df_kind = D_FORWMODULE;
	open_scope(CLOSEDSCOPE);
	vis = CurrVis;		/* The new scope, but watch out, it's "sc_encl"
				   field is not set right. It must indicate the
				   enclosing scope, but this must be done AFTER
				   closing this one
				*/
	df->for_vis = vis;
	df->for_node = MkNode(Name, NULLNODE, NULLNODE, &(idn->nd_token));
	close_scope(0);	
	vis->sc_encl = enclosing(CurrVis);
				/* Here ! */
	return vis;
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
	struct scopelist *vis = enclosing(CurrVis);
	int forwflag = 0;
#define FROM_MODULE	0
#define FROM_ENCLOSING	1
	int imp_kind = FROM_ENCLOSING;
	struct def *lookfor(), *GetDefinitionModule();

	if (idn) {
		imp_kind = FROM_MODULE;
		if (local) {
			df = lookfor(idn, vis, 0);
			switch(df->df_kind) {
			case D_ERROR:
				/* The module from which the import was done
				   is not yet declared. I'm not sure if I must
				   accept this, but for the time being I will.
				   ???
				*/
				vis = ForwModule(df, idn);
				forwflag = 1;
				break;
			case D_FORWMODULE:
				vis = df->for_vis;
				break;
			case D_MODULE:
				vis = df->mod_vis;
				break;
			default:
node_error(idn, "identifier \"%s\" does not represent a module",
idn->nd_IDF->id_text);
				break;
			}
		}
		else	vis = GetDefinitionModule(idn->nd_IDF)->mod_vis;

		FreeNode(idn);
	}

	idn = ids;
	while (ids) {
		if (imp_kind == FROM_MODULE) {
			if (forwflag) {
				df = ForwDef(ids, vis->sc_scope);
			}
			else if (!(df = lookup(ids->nd_IDF, vis->sc_scope))) {
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
			if (local) df = ForwDef(ids, vis->sc_scope);
			else	df = GetDefinitionModule(ids->nd_IDF);
		}

		define(ids->nd_IDF,CurrentScope,D_IMPORT)->imp_def = df;
		DoImport(df, CurrentScope);

		ids = ids->next;
	}

	FreeNode(idn);
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
	static int nmcount = 0;
	extern char *Malloc();
	extern char *strcpy();
	extern char *sprint();
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
		open_scope(OPENSCOPE);
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
			df->prc_vis = CurrVis;
			DefInFront(df);
		}
		else {
			df = define(dot.TOK_IDF, CurrentScope, type);
			if (CurrVis != Defined->mod_vis) {
				sprint(buf, "_%d_%s", ++nmcount,
					df->df_idf->id_text);
			}
			else	sprint(buf, "%s_%s",CurrentScope->sc_name,
						df->df_idf->id_text);
			open_scope(OPENSCOPE);
			df->prc_vis = CurrVis;
			CurrentScope->sc_name = Malloc((unsigned)(strlen(buf)+1));
			strcpy(CurrentScope->sc_name, buf);
			C_inp(buf);
		}
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

AddModule(id)
	struct idf *id;
{
	/*	Add the name of a module to the Module list. This list is
		maintained to create the initialization routine of the
		program/implementation module currently defined.
	*/
	static struct node *nd_end;	/* to remember end of list */
	register struct node *n;
	extern struct node *Modules;

	n = MkNode(Name, NULLNODE, NULLNODE, &dot);
	n->nd_IDF = id;
	n->nd_symb = IDENT;
	if (nd_end) nd_end->next = n;
	nd_end = n;
	if (!Modules) Modules = n;
}

DefInFront(df)
	register struct def *df;
{
	/*	Put definition "df" in front of the list of definitions
		in its scope.
		This is neccessary because in some cases the order in this
		list is important.
	*/
	register struct def *df1;

	if (df->df_scope->sc_def != df) {
		df1 = df->df_scope->sc_def;
		while (df1 && df1->df_nextinscope != df) {
			df1 = df1->df_nextinscope;
		}
		if (df1) df1->df_nextinscope = df->df_nextinscope;
		df->df_nextinscope = df->df_scope->sc_def;
		df->df_scope->sc_def = df;
	}
}

#ifdef DEBUG
PrDef(df)
	register struct def *df;
{
	print("n: %s, k: %d\n", df->df_idf->id_text, df->df_kind);
}
#endif DEBUG
