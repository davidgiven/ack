/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* D E F I N I T I O N   M E C H A N I S M */

/* $Id$ */

#include	"debug.h"

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<assert.h>

#include	"LLlex.h"
#include	"main.h"
#include	"def.h"
#include	"type.h"
#include	"idf.h"
#include	"scope.h"
#include	"node.h"
#include	"Lpars.h"
#include	"warning.h"

extern char *sprint();

STATIC
internal(c)
	register char *c;
{
	if (options['x']) {
		C_exp(c);
	}
	else	C_inp(c);
}

STATIC
DefInFront(df)
	register t_def *df;
{
	/*	Put definition "df" in front of the list of definitions
		in its scope.
		This is neccessary because in some cases the order in this
		list is important.
	*/
	register t_def *df1 = df->df_scope->sc_def;

	if (df1 != df) {
		/* Definition "df" is not in front of the list
		*/
		while (df1) {
			/* Find definition "df"
			*/
			if (df1->df_nextinscope == df) {
				/* It already was in the list. Remove it
				*/
				df1->df_nextinscope = df->df_nextinscope;
				break;
			}
			df1 = df1->df_nextinscope;
		}

		/* Now put it in front
		*/
		df->df_nextinscope = df->df_scope->sc_def;
		df->df_scope->sc_def = df;
	}
}

t_def *
MkDef(id, scope, kind)
	register t_idf *id;
	register t_scope *scope;
{
	/*	Create a new definition structure in scope "scope", with
		id "id" and kind "kind".
	*/
	register t_def *df;

	df = new_def();
	df->df_idf = id;
	df->df_scope = scope;
	df->df_kind = kind;
	df->df_next = id->id_def;
	id->id_def = df;
	if (kind == D_ERROR || kind == D_FORWARD) df->df_type = error_type;
	if (kind & (D_TYPE|D_PROCEDURE|D_CONST)) {
		df->df_flags = D_DEFINED;
	}

	/* enter the definition in the list of definitions in this scope
	*/
	df->df_nextinscope = scope->sc_def;
	scope->sc_def = df;
	return df;
}

t_def *
define(id, scope, kind)
	register t_idf *id;
	register t_scope *scope;
	int kind;
{
	/*	Declare an identifier in a scope, but first check if it
		already has been defined.
		If so, then check for the cases in which this is legal,
		and otherwise give an error message.
	*/
	register t_def *df;

	DO_DEBUG(options['S'], print("define %s, %x\n", id->id_text, kind));
	df = lookup(id, scope, D_IMPORT, 0);
	if (	/* Already in this scope */
		df
	   ) {
		switch(df->df_kind) {
		case D_INUSE:
			if (kind != D_INUSE && kind != D_ERROR) {
				error("identifier \"%s\" already used; may not be redefined in this scope", df->df_idf->id_text);
				df->df_kind = D_ERROR;
				break;
			}
			return df;

		case D_HIDDEN:
			/* An opaque type. We may now have found the
			   definition of this type.
			*/
			if (kind == D_TYPE && df->df_scope == CurrentScope &&
			    !DefinitionModule) {
				df->df_kind = D_TYPE;
				return df;
			}
			break;

		case D_FORWMODULE:
			/* A forward reference to a module. We may have found
			   another one, or we may have found the definition
			   for this module.
			*/
			if (kind & (D_FORWMODULE|D_FORWARD)) {
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

		case D_TYPE:
			if (kind == D_FORWTYPE) return df;
			break;
		case D_FORWTYPE:
			if (kind & (D_FORWTYPE|D_TYPE)) return df;
			error("identifier \"%s\" must be a type", id->id_text);
			df->df_kind = D_ERROR;
			break;
		case D_FORWARD:
			/* A forward reference, for which we may now have
			   found a definition.
			*/
			if (! (kind & (D_FORWARD | D_FORWMODULE))) {
				FreeNode(df->for_node);
			}
			df->df_kind = D_ERROR;	/* avoiding error message */
			break;
		}

		if (kind != D_ERROR && df->df_kind != D_ERROR) {
			/* Avoid spurious error messages
			*/
			error("identifier \"%s\" already declared",
			      id->id_text);
		}
		if (df->df_scope == scope || df->df_kind == D_ERROR) {
			df->df_kind = kind;
			if (kind & (D_TYPE|D_PROCEDURE|D_CONST)) {
				df->df_flags = D_DEFINED;
			}

			return df;
		}
	}

	return MkDef(id, scope, kind);
}

end_definition_list(pdf)
	register t_def **pdf;
{
	/*	Remove all imports from a definition module. This is
		neccesary because the implementation module might import
		them again.
		Also, mark all other definitions "QUALIFIED EXPORT".
	*/
	register t_def *df;

	while (df = *pdf) {
		if (df->df_kind & D_IMPORTED) {
			if (! (df->df_flags & D_USED)) {
				warning(W_ORDINARY, "identifier \"%s\" imported but not used", df->df_idf->id_text);
			}
			RemoveFromIdList(df);
			*pdf = df->df_nextinscope;
			free_def(df);
		}
		else {
			df->df_flags |= D_QEXPORTED;
			pdf = &(df->df_nextinscope);
		}
	}
}

RemoveFromIdList(df)
	register t_def *df;
{
	/*	Remove definition "df" from the definition list
	*/
	register t_idf *id = df->df_idf;
	register t_def *df1;

	if ((df1 = id->id_def) == df) id->id_def = df->df_next;
	else {
		while (df1->df_next != df) {
			assert(df1->df_next != 0);
			df1 = df1->df_next;
		}
		df1->df_next = df->df_next;
	}
}

t_def *
DeclProc(type, id)
	register t_idf *id;
{
	/*	A procedure is declared, either in a definition or a program
		module. Create a def structure for it (if neccessary).
		Also create a name for it.
	*/
	register t_def *df;
	register t_scope *scope;
	static int nmcount;
	char buf[256];

	assert(type & (D_PROCEDURE | D_PROCHEAD));

	if (type == D_PROCHEAD) {
		/* In a definition module
		*/
		df = define(id, CurrentScope, type);
		df->for_node = dot2leaf(Name);
		df->df_flags |= D_USED | D_DEFINED;
		if (CurrentScope->sc_definedby->df_flags & D_FOREIGN) {
			df->prc_name = id->id_text;
		}
		else {
			sprint(buf,"%s_%s",CurrentScope->sc_name,id->id_text);
			df->prc_name = Salloc(buf, (unsigned) (strlen(buf)+1));
		}
		if (CurrVis == Defined->mod_vis) {
			/* The current module will define this routine.
			   make sure the name is exported.
			*/
			C_exp(df->prc_name);
		}
	}
	else {
		df = lookup(id, CurrentScope, D_IMPORTED, 0);
		if (df && df->df_kind == D_PROCHEAD) {
			/* C_exp already generated when we saw the definition
			   in the definition module
			*/
			DefInFront(df);
		}
		else {
			df = define(id, CurrentScope, type);
			sprint(buf,"_%d_%s",++nmcount,id->id_text);
			df->prc_name = Salloc(buf, (unsigned)(strlen(buf)+1));
			internal(buf);
			df->df_flags |= D_DEFINED;
		}
		open_scope(OPENSCOPE);
		scope = CurrentScope;
		scope->sc_name = df->prc_name;
		scope->sc_definedby = df;
	}
	df->prc_vis = CurrVis;

	return df;
}

EndProc(df, id)
	register t_def *df;
	t_idf *id;
{
	/*	The end of a procedure declaration.
		Check that the closing identifier matches the name of the
		procedure, close the scope, and check that a function
		procedure has at least one RETURN statement.
	*/
	extern int return_occurred;

	match_id(id, df->df_idf);
	close_scope(SC_CHKFORW|SC_REVERSE);
	if (! return_occurred && ResultType(df->df_type)) {
		error("function procedure %s does not return a value",
		      df->df_idf->id_text);
	}
}

t_def *
DefineLocalModule(id)
	t_idf *id;
{
	/*	Create a definition for a local module. Also give it
		a name to be used for code generation.
	*/
	register t_def *df = define(id, CurrentScope, D_MODULE);
	register t_scope *sc;
	static int modulecount = 0;
	char buf[256];
	extern int proclevel;

	sprint(buf, "_%d%s_", ++modulecount, id->id_text);

	if (!df->mod_vis) {	
		/* We never saw the name of this module before. Create a
		   scope for it.
		*/
	  	open_scope(CLOSEDSCOPE);
	  	df->mod_vis = CurrVis;
	}

	CurrVis = df->mod_vis;

	sc = CurrentScope;
	sc->sc_level = proclevel;
	sc->sc_definedby = df;
	sc->sc_name = Salloc(buf, (unsigned) (strlen(buf) + 1));

	/* Create a type for it
	*/
	df->df_type = standard_type(T_RECORD, 1, (arith) 0);
	df->df_type->rec_scope = sc;

	/* Generate code that indicates that the initialization procedure
	   for this module is local.
	*/
	internal(buf);
	return df;
}

CheckWithDef(df, tp)
	register t_def *df;
	t_type *tp;
{
	/*	Check the header of a procedure declaration against a
		possible earlier definition in the definition module.
	*/

	if (df->df_kind == D_PROCHEAD &&
	    df->df_type &&
	    df->df_type != error_type) {
		/* We already saw a definition of this type
		   in the definition module.
		*/

	  	if (!TstProcEquiv(tp, df->df_type)) {
			error("inconsistent procedure declaration for \"%s\"",
			      df->df_idf->id_text); 
		}
		FreeType(df->df_type);
		df->df_kind = D_PROCEDURE;
	}
	df->df_type = tp;
}

#ifdef DEBUG
PrDef(df)
	register t_def *df;
{
	print("n: %s, k: %d\n", df->df_idf->id_text, df->df_kind);
}
#endif /* DEBUG */
