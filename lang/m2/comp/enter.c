/* H I G H   L E V E L   S Y M B O L   E N T R Y   A N D   L O O K U P */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

#include	"debug.h"

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
			int xalign = type->tp_align;

			if (xalign < word_align && kind != D_FIELD) {
				xalign = word_align;
			}

			if (*addr >= 0) {
				if (scope->sc_level) {
					/* alignment of parameters is on
					   word boundaries. We cannot do any
					   better, because we don't know the
					   alignment of the stack pointer when
					   starting to push parameters
					*/
					off = *addr;
					*addr = align(off, word_align);
				}
				else {
					/* for global variables we can honour
					   the alignment requirements totally.
					*/
					off = align(*addr, xalign);
					*addr = off + type->tp_size;
				}
			}
			else {
				off = -align(-*addr-type->tp_size, xalign);
				*addr = off;
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
		procedure.
	*/
	register struct def *df;
	register struct scopelist *sc;
	char buf[256];
	extern char *sprint(), *Malloc(), *strcpy();

	sc = CurrVis;

	if (local) {
		/* Find the closest enclosing open scope. This
		   is the procedure that we are dealing with
		*/
		while (sc->sc_scope->sc_scopeclosed) sc = enclosing(sc);
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
			/* subtract aligned size of variable to the offset,
			   as the variable list exists only local to a
			   procedure
			*/
			sc->sc_scope->sc_off =
				-align(type->tp_size - sc->sc_scope->sc_off,
						type->tp_align);
			df->var_off = sc->sc_scope->sc_off;
		}
		else if (!DefinitionModule && CurrVis != Defined->mod_vis) {	
			/* variable list belongs to an internal global
			   module.
			   Align offset and add size
			*/
			sc->sc_scope->sc_off =
				align(sc->sc_scope->sc_off, type->tp_align);
			df->var_off = sc->sc_scope->sc_off;
			df->var_name = 0;
			sc->sc_scope->sc_off += type->tp_size;
		}
		else {
			/* Global name, possibly external
			*/
			sprint(buf,"%s_%s", sc->sc_scope->sc_name,
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
lookfor(id, vis, give_error)
	struct node *id;
	struct scopelist *vis;
{
	/*	Look for an identifier in the visibility range started by
		"vis".
		If it is not defined, maybe give an error message, and
		create a dummy definition.
	*/
	struct def *df;
	register struct scopelist *sc = vis;
	struct def *MkDef();

	while (sc) {
		df = lookup(id->nd_IDF, sc->sc_scope);
		if (df) return df;
		sc = nextvisible(sc);
	}

	if (give_error) id_not_declared(id);

	return MkDef(id->nd_IDF, vis->sc_scope, D_ERROR);
}
