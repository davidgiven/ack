/* H I G H   L E V E L   S Y M B O L   E N T R Y */

#include	"debug.h"

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<assert.h>

#include	"idf.h"
#include	"def.h"
#include	"type.h"
#include	"scope.h"
#include	"LLlex.h"
#include	"node.h"
#include	"main.h"

struct def *
Enter(name, kind, type, pnam)
	char *name;
	struct type *type;
{
	/*	Enter a definition for "name" with kind "kind" and type
		"type" in the Current Scope. If it is a standard name, also
		put its number in the definition structure.
	*/
	register struct def *df;

	df = define(str2idf(name, 0), CurrentScope, kind);
	df->df_type = type;
	if (pnam) df->df_value.df_stdname = pnam;
	return df;
}

EnterEnumList(Idlist, type)
	struct node *Idlist;
	register struct type *type;
{
	/*	Put a list of enumeration literals in the symbol table.
		They all have type "type".
		Also assign numbers to them, and link them together.
		We must link them together because an enumeration type may
		be exported, in which case its literals must also be exported.
		Thus, we need an easy way to get to them.
	*/
	register struct def *df;
	register struct node *idlist = Idlist;

	type->enm_ncst = 0;
	for (; idlist; idlist = idlist->next) {
		df = define(idlist->nd_IDF, CurrentScope, D_ENUM);
		df->df_type = type;
		df->enm_val = (type->enm_ncst)++;
		df->enm_next = type->enm_enums;
		type->enm_enums = df;
	}
	FreeNode(Idlist);
}

EnterFieldList(Idlist, type, scope, addr)
	struct node *Idlist;
	register struct type *type;
	struct scope *scope;
	arith *addr;
{
	/*	Put a list of fields in the symbol table.
		They all have type "type", and are put in scope "scope".
		Mark them as QUALIFIED EXPORT, because that's exactly what
		fields are, you can get to them by qualifying them.
	*/
	register struct def *df;
	register struct node *idlist = Idlist;

	for (; idlist; idlist = idlist->next) {
		df = define(idlist->nd_IDF, scope, D_FIELD);
		df->df_type = type;
		df->df_flags |= D_QEXPORTED;
		df->fld_off = align(*addr, type->tp_align);
		*addr = df->fld_off + type->tp_size;
	}
	FreeNode(Idlist);
}

EnterVarList(Idlist, type, local)
	struct node *Idlist;
	struct type *type;
{
	/*	Enter a list of identifiers representing variables into the
		name list. "type" represents the type of the variables.
		"local" is set if the variables are declared local to a
		procedure.
	*/
	register struct def *df;
	register struct node *idlist = Idlist;
	register struct scopelist *sc;
	char buf[256];
	extern char *sprint();

	sc = CurrVis;

	if (local) {
		/* Find the closest enclosing open scope. This
		   is the procedure that we are dealing with
		*/
		while (sc->sc_scope->sc_scopeclosed) sc = enclosing(sc);
	}

	for (; idlist; idlist = idlist->nd_right) {
		df = define(idlist->nd_IDF, CurrentScope, D_VARIABLE);
		df->df_type = type;
		if (idlist->nd_left) {
			/* An address was supplied
			*/
			df->var_addrgiven = 1;
			df->df_flags |= D_NOREG;
			if (idlist->nd_left->nd_type != card_type) {
				node_error(idlist->nd_left,
					   "Illegal type for address");
			}
			df->var_off = idlist->nd_left->nd_INT;
		}
		else if (local) {
			/* subtract aligned size of variable to the offset,
			   as the variable list exists only local to a
			   procedure
			*/
			sc->sc_scope->sc_off =
				-WA(align(type->tp_size - sc->sc_scope->sc_off,
						type->tp_align));
			df->var_off = sc->sc_scope->sc_off;
		}
		else {
			/* Global name, possibly external
			*/
			sprint(buf,"%s_%s", sc->sc_scope->sc_name,
					    df->df_idf->id_text);
			df->var_name = Salloc(buf, (unsigned)(strlen(buf)+1));
			df->df_flags |= D_NOREG;

 			if (DefinitionModule) {
				if (sc == Defined->mod_vis) {
					C_exa_dnam(df->var_name);
				}
			}
			else {
				C_ina_dnam(df->var_name);
			}
		}
	}
	FreeNode(Idlist);
}

EnterParamList(ppr, Idlist, type, VARp, off)
	struct paramlist **ppr;
	struct node *Idlist;
	struct type *type;
	int VARp;
	arith *off;
{
	/*	Create (part of) a parameterlist of a procedure.
		"ids" indicates the list of identifiers, "tp" their type, and
		"VARp" indicates D_VARPAR or D_VALPAR.
	*/
	register struct paramlist *pr;
	register struct def *df;
	register struct node *idlist = Idlist;
	struct node *dummy = 0;
	static struct paramlist *last;

	if (! idlist) {
		/* Can only happen when a procedure type is defined */
		dummy = Idlist = idlist = MkLeaf(Name, &dot);
	}
	for ( ; idlist; idlist = idlist->next) {
		pr = new_paramlist();
		pr->next = 0;
		if (!*ppr) *ppr = pr;
		else	last->next = pr;
		last = pr;
		if (!DefinitionModule && idlist != dummy) {
			df = define(idlist->nd_IDF, CurrentScope, D_VARIABLE);
			df->var_off = *off;
		}
		else	df = new_def();
		pr->par_def = df;
		df->df_type = type;
		df->df_flags = VARp;

		if (IsConformantArray(type)) {
			/* we need room for the base address and a descriptor
			*/
			*off += pointer_size + 3 * word_size;
		}
		else if (VARp == D_VARPAR) {
			*off += pointer_size;
		}
		else {
			*off += WA(type->tp_size);
		}
	}
	FreeNode(Idlist);
}

STATIC
DoImport(df, scope)
	register struct def *df;
	struct scope *scope;
{
	/*	Definition "df" is imported to scope "scope".
		Handle the case that it is an enumeration type or a module.
	*/

	define(df->df_idf, scope, D_IMPORT)->imp_def = df;

	if (df->df_kind == D_TYPE && df->df_type->tp_fund == T_ENUMERATION) {
		/* Also import all enumeration literals
		*/
		for (df = df->df_type->enm_enums; df; df = df->enm_next) {
			define(df->df_idf, scope, D_IMPORT)->imp_def = df;
		}
	}
	else if (df->df_kind == D_MODULE) {
		/* Also import all definitions that are exported from this
		   module
		*/
		for (df = df->mod_vis->sc_scope->sc_def;
		     df;
		     df = df->df_nextinscope) {
			if (df->df_flags & D_EXPORTED) {
				define(df->df_idf,scope,D_IMPORT)->imp_def = df;
			}
		}
	}
}

STATIC struct scopelist *
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
	close_scope(0);	
	vis->sc_encl = enclosing(CurrVis);
				/* Here ! */
	df->for_vis = vis;
	df->for_node = MkLeaf(Name, &(idn->nd_token));
	return vis;
}

STATIC struct def *
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
		df->for_node = MkLeaf(Name, &(ids->nd_token));
	}
	return df;
}

EnterExportList(Idlist, qualified)
	struct node *Idlist;
{
	/*	From the current scope, the list of identifiers "ids" is
		exported. Note this fact. If the export is not qualified, make
		all the "ids" visible in the enclosing scope by defining them
		in this scope as "imported".
	*/
	register struct node *idlist = Idlist;
	register struct def *df, *df1;

	for (;idlist; idlist = idlist->next) {
		df = lookup(idlist->nd_IDF, CurrentScope);

		if (!df) {
			/* undefined item in export list
			*/
			node_error(idlist,
				   "identifier \"%s\" not defined",
				   idlist->nd_IDF->id_text);
			continue;
		}

		if (df->df_flags & (D_EXPORTED|D_QEXPORTED)) {
			node_error(idlist,
				"multiple occurrences of \"%s\" in export list",
				idlist->nd_IDF->id_text);
		}

		df->df_flags |= qualified;
		if (qualified == D_EXPORTED) {
			/* Export, but not qualified.
			   Find all imports of the module in which this export
			   occurs, and export the current definition to it
			*/
			df1 = CurrentScope->sc_definedby->df_idf->id_def;
			while (df1) {
				if (df1->df_kind == D_IMPORT &&
				    df1->imp_def == CurrentScope->sc_definedby) {
					DoImport(df, df1->df_scope);
				}
				df1 = df1->next;
			}

			/* Also handle the definition as if the enclosing
			   scope imports it.
			*/
			df1 = lookup(idlist->nd_IDF,
				     enclosing(CurrVis)->sc_scope);
			if (df1) {
				/* It was already defined in the enclosing
				   scope. There are two legal possibilities,
				   which are examined below.
				*/
				if (df1->df_kind == D_PROCHEAD &&
				     df->df_kind == D_PROCEDURE) {
					df1->df_kind = D_IMPORT;
					df1->imp_def = df;
					continue;
				}
				if (df1->df_kind == D_HIDDEN &&
				    df->df_kind == D_TYPE) {
					if (df->df_type->tp_fund != T_POINTER) {
						node_error(idlist,
"opaque type \"%s\" is not a pointer type",
							df->df_idf->id_text);
					}
					assert(df1->df_type->next == NULLTYPE);
					df1->df_kind = D_TYPE;
					df1->df_type->next = df->df_type;
					continue;
				}
			}

			DoImport(df, enclosing(CurrVis)->sc_scope);
		}
	}
	FreeNode(Idlist);
}

EnterFromImportList(Idlist, FromDef)
	struct node *Idlist;
	register struct def *FromDef;
{
	/*	Import the list Idlist from the module indicated by Fromdef.
	*/
	register struct node *idlist = Idlist;
	register struct scopelist *vis;
	register struct def *df;
	int forwflag = 0;

	switch(FromDef->df_kind) {
	case D_ERROR:
		/* The module from which the import was done
		   is not yet declared. I'm not sure if I must
		   accept this, but for the time being I will.
		   ???
		*/
		vis = ForwModule(FromDef, FromDef->df_idf);
		forwflag = 1;
		break;
	case D_FORWMODULE:
		vis = FromDef->for_vis;
		break;
	case D_MODULE:
		vis = FromDef->mod_vis;
		break;
	default:
		error("identifier \"%s\" does not represent a module",
		       FromDef->df_idf->id_text);
		break;
	}

	for (; idlist; idlist = idlist->next) {
		if (forwflag) df = ForwDef(idlist, vis->sc_scope);
		else if (! (df = lookup(idlist->nd_IDF, vis->sc_scope))) {
			node_error(idlist, 
			  "identifier \"%s\" not declared in qualifying module",
			  idlist->nd_IDF->id_text);
			df = define(idlist->nd_IDF,vis->sc_scope,D_ERROR);
		}
		else if (! (df->df_flags & (D_EXPORTED|D_QEXPORTED))) {
			node_error(idlist,
			"identifier \"%s\" not exported from qualifying module",
			idlist->nd_IDF->id_text);
			df->df_flags |= D_QEXPORTED;
		}
		DoImport(df, CurrentScope);
	}

	FreeNode(Idlist);
}

EnterImportList(Idlist, local)
	struct node *Idlist;
{
	/*	Import "Idlist" from the enclosing scope.
		An exception must be made for imports of the compilation unit.
		In this case, definition modules must be read for "Idlist".
		This case is indicated by the value 0 of the "local" flag.
	*/
	register struct node *idlist = Idlist;
	struct scope *sc = enclosing(CurrVis)->sc_scope;
	extern struct def *GetDefinitionModule();

	for (; idlist; idlist = idlist->next) {
		DoImport(local ?
				ForwDef(idlist, sc) :
				GetDefinitionModule(idlist->nd_IDF) ,
			 CurrentScope);
	}
	FreeNode(Idlist);
}
