/* H I G H   L E V E L   S Y M B O L   E N T R Y   A N D   L O O K U P */

static char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>
#include	"idf.h"
#include	"def.h"
#include	"type.h"
#include	"scope.h"
#include	"LLlex.h"
#include	"node.h"

struct def *
Enter(name, kind, type, pnam)
	char *name;
	struct type *type;
{
	/*	Enter a definition for "name" with kind "kind" and type
		"type" in the Current Scope. If it is a standard name, also
		put its number in the definition structure.
	*/
	struct idf *id;
	struct def *df;

	id = str2idf(name, 0);
	if (!id) fatal("Out of core");
	df = define(id, CurrentScope, kind);
	df->df_type = type;
	if (kind == D_STDPROC || kind == D_STDFUNC) {
		df->df_value.df_stdname = pnam;
	}
	return df;
}

EnterIdList(idlist, kind, flags, type, scope)
	register struct node *idlist;
	struct type *type;
	struct scope *scope;
{
	/*	Put a list of identifiers in the symbol table.
		They all have kind "kind", and type "type", and are put
		in scope "scope". "flags" initializes the "df_flags" field
		of the definition structure.
		Also assign numbers to enumeration literals, and link
		them together.
	*/
	register struct def *df;
	struct def *first = 0, *last = 0;
	int assval = 0;

	while (idlist) {
		df = define(idlist->nd_IDF, scope, kind);
		df->df_type = type;
		df->df_flags = flags;
		if (kind == D_ENUM) {
			if (!first) first = df;
			df->enm_val = assval++;
			if (last) last->enm_next = df;
			last = df;
		}
		idlist = idlist->next;
	}
	if (last) {
		/* Also meaning : kind == D_ENUM */
		assert(kind == D_ENUM);
		last->enm_next = 0;
		type->enm_enums = first;
		type->enm_ncst = assval;
	}
}

struct def *
lookfor(id, scope, give_error)
	struct node *id;
	struct scope *scope;
{
	/*	Look for an identifier in the visibility range started by
		"scope".
		If it is not defined, give an error message, and
		create a dummy definition.
	*/
	struct def *df;
	register struct scope *sc = scope;

	while (sc) {
		df = lookup(id->nd_IDF, sc->sc_scope);
		if (df) return df;
		sc = nextvisible(sc);
	}
	if (give_error) id_not_declared(id);
	return define(id->nd_IDF, scope, D_ERROR);
}
