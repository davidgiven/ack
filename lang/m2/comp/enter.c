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
	struct idf *id;
	struct def *df;

	id = str2idf(name, 0);
	if (!id) fatal("Out of core");
	df = define(id, CurrentScope, kind);
	df->df_type = type;
	if (type = std_type) {
		df->df_value.df_stdname = pnam;
	}
	return df;
}

EnterIdList(idlist, kind, flags, type, scope, addr)
	register struct node *idlist;
	struct type *type;
	struct scope *scope;
	arith *addr;
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
	arith off;

	while (idlist) {
		df = define(idlist->nd_IDF, scope, kind);
		df->df_type = type;
		df->df_flags |= flags;
		if (addr) {
			if (*addr >= 0) {
				off = align(*addr, type->tp_align);
				*addr = off + type->tp_size;
			}
			else {
				off = -align(-*addr, type->tp_align);
				*addr = off - type->tp_size;
			}
			if (kind == D_VARIABLE) {
				df->var_off = off;
			}
			else {
				assert(kind == D_FIELD);
				df->fld_off = off;
			}
		}
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

EnterVarList(IdList, type, local)
	register struct node *IdList;
	struct type *type;
{
	/*	Enter a list of identifiers representing variables into the
		name list. "type" represents the type of the variables.
		"local" is set if the variables are declared local to a
		procedure
	*/
	register struct def *df;
	register struct scope *scope;
	char buf[256];
	extern char *sprint(), *Malloc(), *strcpy();

	scope = CurrentScope;
	if (local) {
		/* Find the closest enclosing open scope. This
		   is the procedure that we are dealing with
		*/
		while (scope->sc_scopeclosed) scope = scope->next;
	}

	while (IdList) {
		df = define(IdList->nd_IDF, CurrentScope, D_VARIABLE);
		df->df_type = type;
		if (IdList->nd_left) {
			/* An address was supplied
			*/
			df->var_addrgiven = 1;
			if (IdList->nd_left->nd_type != card_type) {
node_error(IdList->nd_left,"Illegal type for address");
			}
			df->var_off = IdList->nd_left->nd_INT;
		}
		else if (local) {
			arith off;

			/* add aligned size of variable to the offset
			*/
			off = scope->sc_off - type->tp_size;
			off = -align(-off, type->tp_align);
			df->var_off = off;
			scope->sc_off = off;
		}
		else if (!DefinitionModule &&
			 CurrentScope != Defined->mod_scope) {	
			scope->sc_off = align(scope->sc_off, type->tp_align);
			df->var_off = scope->sc_off;
			scope->sc_off += type->tp_size;
		}
		else {
			sprint(buf,"%s_%s", df->df_scope->sc_name,
					    df->df_idf->id_text);
			df->var_name = Malloc((unsigned)(strlen(buf)+1));
			strcpy(df->var_name, buf);
 			if (DefinitionModule) {
				C_exa_dnam(df->var_name);
			}
			else {
				C_ina_dnam(df->var_name);
			}
		}
		IdList = IdList->nd_right;
	}
}

struct def *
lookfor(id, scope, give_error)
	struct node *id;
	struct scope *scope;
{
	/*	Look for an identifier in the visibility range started by
		"scope".
		If it is not defined, maybe give an error message, and
		create a dummy definition.
	*/
	struct def *df;
	register struct scope *sc = scope;
	struct def *MkDef();

	while (sc) {
		df = lookup(id->nd_IDF, sc);
		if (df) return df;
		sc = nextvisible(sc);
	}

	if (give_error) id_not_declared(id);

	return MkDef(id->nd_IDF, scope, D_ERROR);
}
