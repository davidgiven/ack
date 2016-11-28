/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	IDENTIFIER  FIDDLING & SYMBOL TABLE HANDLING	*/

#include	<stdlib.h>
#include	<string.h>
#include	"parameters.h"
#include	<em_reg.h>
#include	<alloc.h>
#include    "idf.h"
#include	"arith.h"
#include	"align.h"
#include	"LLlex.h"
#include	"level.h"
#include	"stack.h"
#include	"label.h"
#include	"def.h"
#include	"type.h"
#include	"proto.h"
#include	"struct.h"
#include	"declar.h"
#include	"decspecs.h"
#include	"sizes.h"
#include	"Lpars.h"
#include	"assert.h"

extern char options[];
extern arith NewLocal();
extern char *symbol2str();

#ifdef DEBUG
#define IDF_DEBUG
#endif

#include <idf_pkg.body>

void global_redecl();

struct idf *
gen_idf()
{
	/*	A new idf is created out of nowhere, to serve as an
		anonymous name.
	*/
	static int name_cnt;
	char *s = Malloc(strlen(dot.tk_file) + 50);

	sprint(s, "#%d in %s, line %u",
			++name_cnt, dot.tk_file, dot.tk_line);
	s = Realloc(s, strlen(s)+1);
	return str2idf(s, 0);
}

int
is_anon_idf(idf)
	struct idf *idf;
{
	return idf->id_text[0] == '#';
}

declare_idf(ds, dc, lvl)
	struct decspecs *ds;
	struct declarator *dc;
{
	/*	The identifier inside dc is declared on the level lvl, with
		properties deduced from the decspecs ds and the declarator
		dc.
		The level is given explicitly to be able to insert, e.g.,
		labels on the outermost level inside the function.
		This routine implements the rich semantics of C
		declarations.
	*/
	register struct idf *idf = dc->dc_idf;
	register int sc = ds->ds_sc;
		/*	This local copy is essential:
				char b(), c;
			makes b GLOBAL and c AUTO.
		*/
	register struct def *def = idf->id_def;		/* may be NULL */
	register struct type *type;
	struct stack_level *stl = stack_level_of(lvl);
	char formal_array = 0;
	
	/* determine the present type */
	if (ds->ds_type == 0)	{
		/*	at the L_FORMAL1 level there is no type specified yet
		*/
		ASSERT(lvl == L_FORMAL1);
		type = int_type;	/* may change at L_FORMAL2 */
	}
	else	{
		/* combine the decspecs and the declarator into one type */
		type = declare_type(ds->ds_type, dc);
		if (type->tp_size <= (arith)0 &&
		    actual_declaration(sc, type))	{
			if (type->tp_size == (arith) -1) {
				/* the type is not yet known,
				   but it has to be:
				*/
				if (type->tp_fund != VOID) {
				    if (level != L_GLOBAL)
					error("unknown %s-type",
						symbol2str(type->tp_fund));
				} else	error("void is not a complete type");
			}
			else strict("%s has size 0", idf->id_text);
		}
	}

	/* some additional work for formal definitions */
	if (lvl == L_FORMAL2)	{
		switch (type->tp_fund)	{
		case FUNCTION:
			warning("%s is a function; cannot be formal",
				idf->id_text);
			type = construct_type(POINTER, type, 0, (arith)0,
					      NO_PROTO);
			break;
		case ARRAY:	/* 3.7.1 */
			type = construct_type(POINTER, type->tp_up, 0, (arith)0,
					      NO_PROTO);
			formal_array = 1;
			break;
		case FLOAT:
		case CHAR:
		case SHORT:
			/* The conversion is done in formal_cvt(). It is
			 * not done when the type is float and there is a
			 * prototype.
			 */
			break;
		}
	}
	/*	The tests on types, postponed from do_decspecs(), can now
		be performed.
	*/
	/* update the storage class */
	if (type && type->tp_fund == FUNCTION)	{
		if (lvl != L_GLOBAL)  {		/* 3.5.1 */
			if (sc == 0)
				sc = GLOBAL;
			else if (sc != EXTERN && sc != TYPEDEF) {
				error("illegal storage class %s for function with block-scope"
					, symbol2str(sc));
				ds->ds_sc = sc = EXTERN;
			}
		}
		else if (sc == 0)
			sc = GLOBAL;
	}
	else	/* non-FUNCTION */
		if (sc == 0)
			sc =	lvl == L_GLOBAL ? GLOBAL
				: lvl == L_FORMAL1 || lvl == L_FORMAL2 ? FORMAL
				: AUTO;

#ifdef	LINT
	check_hiding(idf, lvl, sc);	/* of some idf by this idf */
#endif	/* LINT */
	if (def && lvl == L_LOCAL && def->df_level == L_FORMAL2) {
		error("%s redeclared", idf->id_text);
	}

	if (def && 
	    ( def->df_level == lvl ||
	      ( lvl != L_GLOBAL && def->df_level > lvl ) ||
	      (lvl == L_GLOBAL
	       && def->df_level == L_PROTO
	       && def->next && def->next->df_level == L_GLOBAL)
	   ))	{
		/*	There is already a declaration for idf on this
			level, or even more inside.
			The rules differ for different levels.
		*/
		switch (lvl)	{
		case L_GLOBAL:
			global_redecl(idf, sc, type);
			def->df_file = idf->id_file;
			def->df_line = idf->id_line;
			break;
		case L_FORMAL1:	/* formal declaration */
			error("formal %s redeclared", idf->id_text);
			break;
		case L_FORMAL2:	/* formal definition */
		default:	/* local */
			if (sc != EXTERN) error("%s redeclared", idf->id_text);
			break;
		}
	}
	else	/* the idf is unknown on this level */
	if (lvl == L_FORMAL2 && sc != ENUM && good_formal(def, idf))	{
		/* formal declaration, update only */
		def->df_type = type;
		def->df_formal_array = formal_array;
		def->df_sc = sc;
		def->df_level = L_FORMAL2;	/* CJ */
		def->df_file = idf->id_file;
		def->df_line = idf->id_line;
	}
	else	{ /* fill in the def block */
		register struct def *newdef = new_def();

		newdef->next = def;
		newdef->df_level = lvl;
		newdef->df_type = type;
		newdef->df_sc = sc;
		newdef->df_file = idf->id_file;
		newdef->df_line = idf->id_line;
#ifdef	LINT
		newdef->df_set = 0;
		newdef->df_firstbrace = 0;
#endif	/* LINT */
		/* link it into the name list in the proper place */
		idf->id_def = newdef;
		update_ahead(idf);
		stack_idf(idf, stl);
		/*	We now calculate the address.
			Globals have names and don't get addresses, they
			get numbers instead (through data_label()).
			Formals are handled by declare_formals().
			So here we hand out local addresses only.
		*/
		if (lvl >= L_LOCAL)	{
			ASSERT(sc);
			switch (sc)	{
			case REGISTER:
			case AUTO:
				if (type->tp_size == (arith)-1
					&& type->tp_fund != ARRAY) {
					error("size of local %s unknown",
						idf->id_text);
				/** type = idf->id_def->df_type = int_type; **/
				}
				if (type->tp_size != (arith) -1) {
				    newdef->df_address =
					NewLocal(type->tp_size,
						 type->tp_align,
						 regtype(type),
						 sc);
				}
				break;
			case STATIC:
				newdef->df_address = (arith) data_label();
				break;
			}
		}
	}
}

int
actual_declaration(sc, tp)
	int sc;
	struct type *tp;
{
	/*	An actual_declaration needs space, right here and now.
	*/
	register int fund = tp->tp_fund;
	
	if (sc == ENUM || sc == TYPEDEF) /* virtual declarations */
		return 0;
	if (fund == FUNCTION || fund == ARRAY)
		/* allocation solved in other ways */
		return 0;
	if (sc == EXTERN && fund == VOID) {
		/* strange, but should be accepted */
		return 0;
	}
	/* to be allocated */
	return 1;
}

void
global_redecl(idf, new_sc, tp)
	register struct idf *idf;
	struct type *tp;
{
	/*	A global identifier may be declared several times,
		provided the declarations do not conflict; they might
		conflict in type (or supplement each other in the case of
		an array) or they might conflict or supplement each other
		in storage class.
	*/
	register struct def *def = idf->id_def;

	while (def->df_level != L_GLOBAL) def = def->next;
	if (!equal_type(tp, def->df_type, 0, 1)) {
		error("redeclaration of %s with different type", idf->id_text);
		return;
	} else	update_proto(tp, def->df_type);
	if (tp->tp_fund == ARRAY) {
		/* Multiple array declaration; this may be interesting */
		if (tp->tp_size < 0)	{		/* new decl has [] */
			/* nothing new */
		} else
		if (def->df_type->tp_size < 0)	{	/* old decl has [] */
			def->df_type = tp;
		}
	} if (tp->tp_fund == FUNCTION && new_sc == GLOBAL) {
		/* see 3.1.2.2 */
		new_sc = EXTERN;
	}

	/*	Now we may be able to update the storage class.
		Clean out this mess as soon as we know all the possibilities
		for new_sc.
		For now we have:
			EXTERN:		we have seen the word "extern"
			GLOBAL:		the item was declared on the outer
					level, without either "extern" or
					"static".
			STATIC:		we have seen the word "static"
	*/

	switch (def->df_sc)	{	/* the old storage class */
	case EXTERN:
		switch (new_sc)	{	/* the new storage class */
		case STATIC:
			warning("%s redeclared static", idf->id_text);
			/* fallthrough */
		case GLOBAL:
			def->df_sc = new_sc;
			/* fallthrough */
		case EXTERN:
			break;
		default:
			crash("bad storage class");
			/*NOTREACHED*/
		}
		break;
	case GLOBAL:
		switch (new_sc)	{	/* the new storage class */
		case STATIC:		/* linkage disagreement */
			warning("%s redeclared static", idf->id_text);
			def->df_sc = new_sc;
			/* fallthrough */
		case GLOBAL:
		case EXTERN:
			break;
		default:
			crash("bad storage class");
			/*NOTREACHED*/
		}
		break;
	case STATIC:
		switch (new_sc)	{	/* the new storage class */
		case GLOBAL:		/* linkage disagreement */
		case EXTERN:
			warning("%s is already declared static", idf->id_text);
			/* fallthrough */
		case STATIC:
			break;
		default:
			crash("bad storage class");
			/*NOTREACHED*/
		}
		break;
	case ENUM:
	case TYPEDEF:
		error("illegal redeclaration of %s", idf->id_text);
		break;
	default:
		crash("bad storage class");
		/*NOTREACHED*/
	}
}

int
good_formal(def, idf)
	register struct def *def;
	register struct idf *idf;
{
	/*	Succeeds if def is a proper L_FORMAL1 definition and
		gives an error message otherwise.
	*/
	if (!def || def->df_level != L_FORMAL1)	{ /* not in parameter list */
		if (!is_anon_idf(idf))
			error("%s not in parameter list", idf->id_text);
		return 0;
	}
	ASSERT(def->df_sc == FORMAL);	/* CJ */
	return 1;
}

declare_params(dc)
	struct declarator *dc;
{
	/*	Declares the formal parameters if they exist.
	*/
	register struct formal *fm = dc->dc_formal;

	while (fm)	{
		declare_parameter(fm->fm_idf);
		fm = fm->next;
	}
}

void
idf_initialized(idf)
	register struct idf *idf;
{
	/*	The topmost definition of idf is set to initialized.
	*/
	register struct def *def = idf->id_def;	/* the topmost */
	
	while (def->df_level <= L_PROTO) def = def->next;
	if (def->df_initialized)
		error("multiple initialization of %s", idf->id_text);
	if (def->df_sc == TYPEDEF)	{
		error("typedef cannot be initialized");
		return;
	}
	def->df_initialized = 1;
}

declare_parameter(idf)
	struct idf *idf;
{
	/*	idf is declared as a formal.
	*/
	add_def(idf, FORMAL, int_type, level);
}

declare_enum(tp, idf, l)
	struct type *tp;
	struct idf *idf;
	arith l;
{
	/*	idf is declared as an enum constant with value l.
	*/
	add_def(idf, ENUM, tp, level);
	idf->id_def->df_address = l;
}

void
check_formals(idf, dc)
	struct idf *idf;
	struct declarator *dc;
{
	register struct formal *fm = dc->dc_formal;
	register struct proto *pl = idf->id_def->df_type->tp_proto;
	register struct decl_unary *du = dc->dc_decl_unary;

	if (!du) {	/* error or typdef'ed function */
		error("illegal definition of %s", idf->id_text);
		return;
	}

	while (du
		&& (du->du_fund != FUNCTION
		    || du->next != (struct decl_unary *) 0)) {
		du = du->next;
	}
	if (!du) return;	/* terrible error, signalled earlier */

	if (du->du_proto) return;

	if (pl) {
		/* Don't give a warning about an old-style definition,
		 * since the arguments will be checked anyway.
		 */
		if (pl->pl_flag & PL_ELLIPSIS) {
		    if (!(du->du_proto) && !(pl->pl_flag & PL_ERRGIVEN))
			error("ellipsis terminator in previous declaration");
		    pl = pl->next;
		}
		else if (pl->pl_flag & PL_VOID) {
		    pl = pl->next;			/* should be 0 */
		}
		while(fm && pl) {
		    if (!equal_type(promoted_type(fm->fm_idf->id_def->df_type)
					, pl->pl_type, -1, 1)) {
			if (!(pl->pl_flag & PL_ERRGIVEN))
			    error("incorrect type for parameter %s"
						, fm->fm_idf->id_text);
			pl->pl_flag |= PL_ERRGIVEN;
		    }
		    fm = fm->next;
		    pl = pl->next;
		}
		if (pl || fm) {
			error("incorrect number of parameters");
		}
	} else {			/* make a pseudo-prototype */
		register struct proto *lpl = new_proto();

		if (!options['o'])
			warning("'%s' old-fashioned function definition"
					, dc->dc_idf->id_text);

		while (fm) {
			if (pl == 0) pl = lpl;
			else {
				lpl->next = new_proto();
				lpl = lpl->next;
			}
			lpl->pl_flag = PL_FORMAL;
			lpl->pl_idf = fm->fm_idf;
			lpl->pl_type =
				    promoted_type(fm->fm_idf->id_def->df_type);
			fm = fm->next;
		}
		if (pl == 0) {		/* make func(void) */
			pl = lpl;
			pl->pl_type = void_type;
			pl->pl_flag = PL_FORMAL | PL_VOID;
		}
		idf->id_def->df_type->tp_pseudoproto = pl;
	}
	free_formals(dc->dc_formal);
	dc->dc_formal = 0;
}

declare_formals(idf, fp)
	struct idf *idf;
	arith *fp;
{
	/*	Declares those formals as int that haven't been declared
		by the user.
		An address is assigned to each formal parameter.
		The total size of the formals is returned in *fp;
	*/
	register struct stack_entry *se = stack_level_of(L_FORMAL1)->sl_entry;
	arith f_offset = (arith)0;
	register int nparams = 0;
	int hasproto;
	struct def *df = idf->id_def;

	/* When one of the formals has the same name as the function, 
	   it hides the function def. Get it.
	*/
	while (se) {
		if (se->se_idf == idf) {
			df = df->next;
			break;
		}
		se = se->next;
	}

	se = stack_level_of(L_FORMAL1)->sl_entry;
	
	hasproto = df->df_type->tp_proto != 0;

#ifdef	DEBUG
	if (options['t'])
		dumpidftab("start declare_formals", 0);
#endif	/* DEBUG */
	if (is_struct_or_union(df->df_type->tp_up->tp_fund)) {
		/* create space for address of return value */
		f_offset = pointer_size;
	}
	while (se)	{
		df = se->se_idf->id_def;
		
		/* this stacklevel may also contain tags. ignore them */
		if (!df || df->df_level < L_FORMAL1 ) {
			se = se->next;
			continue;
		}

		df->df_address = f_offset;
		/*	the alignment convention for parameters is: align on
			word boundaries, i.e. take care that the following
			parameter starts on a new word boundary.
		*/
		if (! hasproto 
		    && df->df_type->tp_fund == FLOAT
		    && df->df_type->tp_size != double_size) {
			f_offset = align(f_offset + double_size, (int) word_size);
		}
		else f_offset = align(f_offset + df->df_type->tp_size, (int) word_size);
		RegisterAccount(df->df_address, df->df_type->tp_size,
				regtype(df->df_type),
				df->df_sc);
		/* cvt int to char or short and double to float, if necessary
		 */
		formal_cvt(hasproto, df);

		df->df_level = L_FORMAL2;	/* CJ */
		if (nparams++ >= STDC_NPARAMS)
			strict("number of formal parameters exceeds ANSI limit");
#ifdef DBSYMTAB
		if (options['g']) {
			stb_string(df, FORMAL, se->se_idf->id_text);
		}
#endif /* DBSYMTAB */
		se = se->next;
	}
	*fp = f_offset;
}

int
regtype(tp)
	struct type *tp;
{
	switch(tp->tp_fund) {
	case INT:
	case LONG:
		return reg_any;
	case FLOAT:
	case DOUBLE:
	case LNGDBL:
		return reg_float;
	case POINTER:
		return reg_pointer;
	}
	return -1;
}

add_def(idf, sc, tp, lvl)
	struct idf *idf;
	struct type *tp;
	int lvl;
	int sc;
{
	/*	The identifier idf is declared on level lvl with storage
		class sc and type tp, through a faked C declaration.
		This is probably the wrong way to structure the problem,
		but it will have to do for the time being.
	*/
	struct decspecs Ds; struct declarator Dc;

	Ds = null_decspecs;
	Ds.ds_type = tp;
	Ds.ds_sc = sc;
	Dc = null_declarator;
	Dc.dc_idf = idf;
	declare_idf(&Ds, &Dc, lvl);
}

update_ahead(idf)
	register struct idf *idf;
{
	/*	The tk_symb of the token ahead is updated in the light of new
		information about the identifier idf.
	*/
	register int tk_symb = AHEAD;

	if (	(tk_symb == IDENTIFIER || tk_symb == TYPE_IDENTIFIER) &&
		ahead.tk_idf == idf
	)
		AHEAD = idf->id_def && idf->id_def->df_sc == TYPEDEF ?
				TYPE_IDENTIFIER : IDENTIFIER;
}

free_formals(fm)
	register struct formal *fm;
{
	while (fm)	{
		struct formal *tmp = fm->next;

		free_formal(fm);
		fm = tmp;
	}
}
