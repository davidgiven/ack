/* H I G H   L E V E L   S Y M B O L   E N T R Y */

#include	<alloc.h>
#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"LLlex.h"
#include	"def.h"
#include	"idf.h"
#include	"main.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"
#include	"dbsymtab.h"

extern int	proclevel;
extern int	parlevel;

struct def *
Enter(name, kind, type, pnam)
	char *name;
	register struct type *type;
	long kind;
{
	/*	Enter a definition for "name" with kind "kind" and type
		"type" in the Current Scope. If it is a standard name, also
		put its number in the definition structure, and mark the
		name as set, to inhibit warnings about used before set.
	*/
	register struct def *df;

	df = define(str2idf(name, 0), CurrentScope, kind);
	df->df_type = type;
	if( pnam ) {
		df->df_value.df_reqname = pnam;
		df->df_flags |= D_SET;
	}
#ifdef DBSYMTAB
	else if (options['g']) stb_string(df, kind);
#endif /*  DBSYMTAB */
	return df;
}

EnterProgList(Idlist)
	register struct node *Idlist;
{
	register struct node *idlist = Idlist;
	register struct def *df;

	for( ; idlist; idlist = idlist->nd_next )
		if (	!strcmp(input, idlist->nd_IDF->id_text)
			||
			!strcmp(output, idlist->nd_IDF->id_text)
		   ) {
			/* the occurence of input or output as program- 
			 * parameter is their declaration as a GLOBAL
			 *  variable of type text
			 */
			if( df = define(idlist->nd_IDF, CurrentScope,
							D_VARIABLE) )	{
				df->df_type = text_type;
				df->df_flags |= (D_SET | D_PROGPAR | D_NOREG);
				if( !strcmp(input, idlist->nd_IDF->id_text) ) {
					df->var_name = input;
					set_inp();
				}
				else {
					df->var_name = output;
					set_outp();
				}
#ifdef DBSYMTAB
				if (options['g']) stb_string(df, D_VARIABLE);
#endif /*  DBSYMTAB */
			}
		}
		else	{
			if( df = define(idlist->nd_IDF, CurrentScope,
								D_PARAMETER) ) {
				df->df_type = error_type;
				df->df_flags |= D_PROGPAR;
				df->var_name = idlist->nd_IDF->id_text;
			}
		}
	
	FreeNode(Idlist);
}

EnterEnumList(Idlist, type)
	struct node *Idlist;
	register struct type *type;
{
	/*	Put a list of enumeration literals in the symbol table.
		They all have type "type". Also assign numbers to them.
	*/
	register struct def *df, *df1 = 0;
	register struct node *idlist = Idlist;

	type->enm_ncst = 0;
	for( ; idlist; idlist = idlist->nd_next )
		if( df = define(idlist->nd_IDF, CurrentScope, D_ENUM) )	{
			df->df_type = type;
			df->enm_val = (type->enm_ncst)++;
			df->df_flags |= D_SET;
			if (! df1) {
				type->enm_enums = df;
			}
			else	df1->enm_next = df;
			df1 = df;
		}
	FreeNode(Idlist);
}

EnterFieldList(Idlist, type, scope, addr, packed)
	struct node *Idlist;
	register struct type *type;
	struct scope *scope;
	arith *addr;
	unsigned short packed;
{
	/*	Put a list of fields in the symbol table.
		They all have type "type", and are put in scope "scope".
	*/
	register struct def *df;
	register struct node *idlist = Idlist;

	for( ; idlist; idlist = idlist->nd_next )
		if( df = define(idlist->nd_IDF, scope, D_FIELD) )	{
			df->df_type = type;
			if( packed )	{
				df->fld_flags |= F_PACKED;
				df->fld_off = align(*addr, type->tp_palign);
				*addr = df->fld_off + type->tp_psize;
			}
			else	{
				df->fld_off = align(*addr, type->tp_align);
				*addr = df->fld_off + type->tp_size;
			}
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
	register struct scopelist *sc = CurrVis;

	for( ; idlist; idlist = idlist->nd_next )	{
		if( !(df = define(idlist->nd_IDF, CurrentScope, D_VARIABLE)) )
			continue;	/* skip this identifier */
		df->df_type = type;
		if( local )	{
			/* subtract size, which is already aligned, of
			 * variable to the offset, as the variable list
			 * exists only local to a procedure
			 */
			sc->sc_scope->sc_off -= type->tp_size;
			df->var_off = sc->sc_scope->sc_off;
		}
		else	{ /* Global name */
			df->var_name = df->df_idf->id_text;
			df->df_flags |= D_NOREG;
		}
#ifdef DBSYMTAB
		if (options['g']) stb_string(df, D_VARIABLE);
#endif /*  DBSYMTAB */
	}
	FreeNode(Idlist);
}

arith
EnterParamList(fpl, parlist)
	register struct node *fpl;
	struct paramlist **parlist;
{
	register arith nb_pars = (proclevel > 1) ? pointer_size : 0;
	register struct node *id;
	struct type *tp;
	struct def *df;

	for( ; fpl; fpl = fpl->nd_right )	{
		assert(fpl->nd_class == Link);

		tp = fpl->nd_type;
		for( id = fpl->nd_left; id; id = id->nd_next )
		    if( df = define(id->nd_IDF, CurrentScope, D_VARIABLE) ) {
			df->var_off = nb_pars;
			if( fpl->nd_INT & D_VARPAR || IsConformantArray(tp) )
				nb_pars += pointer_size;
			else
				nb_pars += tp->tp_size;
			LinkParam(parlist, df);
			df->df_type = tp;
			df->df_flags |= fpl->nd_INT;
		    }

		while( IsConformantArray(tp) )	{
			/* we need room for the descriptors */

			tp->arr_sclevel = CurrentScope->sc_level;
			tp->arr_cfdescr = nb_pars;
			nb_pars += 3 * word_size;
			tp = tp->arr_elem;
    		}
	}
	return nb_pars;
}

arith
EnterParTypes(fpl, parlist)
	register struct node *fpl;
	struct paramlist **parlist;
{
	/* Parameters in heading of procedural and functional
	   parameters (only types are important, not the names).
	*/
	register arith nb_pars = 0;
	register struct node *id;
	struct type *tp;
	struct def *df;

	for( ; fpl; fpl = fpl->nd_right ) {
		tp = fpl->nd_type;
		for( id = fpl->nd_left; id; id = id->nd_next )
			if( df = new_def() )	{
				if( fpl->nd_INT & D_VARPAR ||
				    IsConformantArray(tp) )
					nb_pars += pointer_size;
				else
					nb_pars += tp->tp_size;
				LinkParam(parlist, df);
				df->df_type = tp;
				df->df_flags |= fpl->nd_INT;
			}
		while( IsConformantArray(tp) ) {
			nb_pars += 3 * word_size;
			tp = tp->arr_elem;
		}
	}
	return nb_pars;
}

LinkParam(parlist, df)
	struct paramlist **parlist;
	struct def *df;
{
	static struct paramlist *pr;

	if( !*parlist )
		*parlist = pr = new_paramlist();
	else	{
		pr->next = new_paramlist();
		pr = pr->next;
	}
	pr->par_def = df;
}
