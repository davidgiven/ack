/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	IDENTIFIER  FIDDLING & SYMBOL TABLE HANDLING	*/

#include	"lint.h"
#include	<em_reg.h>
#include	"nofloat.h"
#include	"debug.h"
#include	"idfsize.h"
#include	"botch_free.h"
#include	"nopp.h"
#include	<alloc.h>
#include	"arith.h"
#include	"align.h"
#include	"LLlex.h"
#include	"level.h"
#include	"stack.h"
#include	"idf.h"
#include	"label.h"
#include	"def.h"
#include	"type.h"
#include	"struct.h"
#include	"declar.h"
#include	"decspecs.h"
#include	"sizes.h"
#include	"Lpars.h"
#include	"assert.h"
#include	"specials.h"	/* registration of special identifiers	*/
#include	"noRoption.h"

int idfsize = IDFSIZE;
extern char options[];
extern arith NewLocal();

char sp_occurred[SP_TOTAL+1];	/* indicate occurrence of special id	*/

struct idf *idf_hashtable[HASHSIZE];
	/*	All identifiers can in principle be reached through
		idf_hashtable; idf_hashtable[hc] is the start of a chain of
		idf's whose tags all hash to hc. Each idf is the start of
		a chain of def's for that idf, sorted according to level,
		with the most recent one on top.
		Any identifier occurring on a level is entered into this
		list, regardless of the nature of its declaration
		(variable, selector, structure tag, etc.).
	*/

struct idf *
idf_hashed(tg, size, hc)
	char *tg;
	int size;		/* includes the '\0' character */
	int hc;
{
	/*	The tag tg with length size and known hash value hc is
		looked up in the identifier table; if not found, it is
		entered. A pointer to it is returned.
		The identifier has already been truncated to idfsize
		characters.
	*/
	register struct idf **hook = &idf_hashtable[hc], *notch;

	while ((notch = *hook))	{
		register char *s1 = tg;
		register char *cp = notch->id_text;
		register int cmp;

		while (!(cmp = (*s1 - *cp++))) {
			if (*s1++ == '\0') {
				break;
			}
		}

		if (cmp < 0)
			break;
		if (cmp == 0)	{
			/*	suppose that special identifiers, as
				"setjmp", are already inserted
			*/
			sp_occurred[notch->id_special] = 1;
			return notch;
		}
		hook = &notch->next;
	}
	/* a new struct idf must be inserted at the hook */
	notch = new_idf();
	notch->next = *hook;
	*hook = notch;		/* hooked in */
	notch->id_text = Salloc(tg, (unsigned) size);
#ifndef NOPP
	notch->id_resmac = 0;
#endif NOPP
	return notch;
}

#ifdef	DEBUG
hash_stat()
{
	if (options['h'])	{
		register int i;

		print("Hash table tally:\n");
		for (i = 0; i < HASHSIZE; i++)	{
			register struct idf *notch = idf_hashtable[i];
			int cnt = 0;

			while (notch)	{
				cnt++;
				notch = notch->next;
			}
			print("%d %d\n", i, cnt);
		}
		print("End hash table tally\n");
	}
}
#endif	DEBUG

struct idf *
str2idf(tg)
	char tg[];
{
	/*	str2idf() returns an entry in the symbol table for the
		identifier tg.	If necessary, an entry is created.
		It is used where the text of the identifier is available
		but its hash value is not; otherwise idf_hashed() is to
		be used.
	*/
	register char *cp = tg;
	register int hash;
	register int pos = -1;
	register int ch;
	char ntg[IDFSIZE + 1];
	register char *ncp = ntg;

	hash = STARTHASH();
	while (++pos < idfsize && (ch = *cp++))	{
		*ncp++ = ch;
		hash = ENHASH(hash, ch, pos);
	}
	hash = STOPHASH(hash);
	*ncp++ = '\0';
	return idf_hashed(ntg, ncp - ntg, hash);
}

struct idf *
gen_idf()
{
	/*	A new idf is created out of nowhere, to serve as an
		anonymous name.
	*/
	static int name_cnt;
	char buff[100];

	sprint(buff, "#%d in %s, line %u",
			++name_cnt, dot.tk_file, dot.tk_line);
	return str2idf(buff);
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
				extern char *symbol2str();
				error("unknown %s-type",
					symbol2str(type->tp_fund));
			}
			else if (type->tp_fund != LABEL) {
				/* CJ */
				warning("%s has size 0", idf->id_text);
			}
		}
	}

	/* some additional work for formal definitions */
	if (lvl == L_FORMAL2)	{
		switch (type->tp_fund)	{
		case FUNCTION:
			warning("%s is a function; cannot be formal",
				idf->id_text);
			type = construct_type(POINTER, type, (arith)0);
			break;
		case ARRAY:	/* RM 10.1	*/
			type = construct_type(POINTER, type->tp_up, (arith)0);
			formal_array = 1;
			break;
#ifndef NOFLOAT
		case FLOAT:	/* RM 10.1	*/
			type = double_type;
			break;
#endif NOFLOAT
		case CHAR:
		case SHORT:
			/*	The RM is not clear about this: we must
				convert the parameter from int (they have
				been pushed as ints) to the specified type.
				The conversion to type int or uint is not
				allowed.
			*/
			break;
		}
	}
	/*	The tests on types, postponed from do_decspecs(), can now
		be performed.
	*/
	/* update the storage class */
	if (type && type->tp_fund == FUNCTION)	{
		if (sc == 0 || (ds->ds_sc_given && sc == AUTO))	/* RM 8.1 */
			sc = GLOBAL;
		else
		if (sc == REGISTER) {
			error("function storage class cannot be register");
			ds->ds_sc = sc = GLOBAL;
		}
	}
	else	/* non-FUNCTION */
		if (sc == 0)
			sc =	lvl == L_GLOBAL ? GLOBAL
				: lvl == L_FORMAL1 || lvl == L_FORMAL2 ? FORMAL
				: AUTO;
#ifndef NOROPTION
	if (options['R']) { /* some special K & R tests */
		/* is it also an enum? */
		if (idf->id_enum && idf->id_enum->tg_level == level)
			warning("%s is also an enum tag", idf->id_text);
		/* is it a universal typedef? */
		if (def && def->df_level == L_UNIVERSAL)
			warning("redeclaring reserved word %s", idf->id_text);
	}
#endif

#ifdef	LINT
	check_hiding(idf, lvl, sc);	/* of some idf by this idf */
#endif	LINT

	if (def &&
	    ( def->df_level == lvl ||
	      ( lvl != L_GLOBAL && def->df_level > lvl )
	    )
	   )	{
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
			error("%s redeclared", idf->id_text);
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
	else
	if (	lvl >= L_LOCAL &&
		(type->tp_fund == FUNCTION || sc == EXTERN)
	)	{
		/*	extern declaration inside function is treated the
			same way as global extern declaration
		*/
#ifndef NOROPTION
		if (	options['R'] &&
			(sc == STATIC && type->tp_fund == FUNCTION)
		)
			if (!is_anon_idf(idf))
				warning("non-global static function %s",
					idf->id_text);
#endif
		declare_idf(ds, dc, L_GLOBAL);
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
#endif	LINT

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
				if (type->tp_size == (arith)-1) {
					error("size of local %s unknown",
						idf->id_text);
				/** type = idf->id_def->df_type = int_type; **/
				}
				newdef->df_address =
					NewLocal(type->tp_size,
						 type->tp_align,
						 regtype(type),
						 sc);
				break;
			case STATIC:
				newdef->df_address = (arith) data_label();
				break;
			}
		}
	}
}

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
	/* to be allocated */
	return 1;
}

global_redecl(idf, new_sc, tp)
	register struct idf *idf;
	register struct type *tp;
{
	/*	A global identifier may be declared several times,
		provided the declarations do not conflict; they might
		conflict in type (or supplement each other in the case of
		an array) or they might conflict or supplement each other
		in storage class.
	*/
	register struct def *def = idf->id_def;

	if (tp != def->df_type)	{
		register struct type *otp = def->df_type;
		if (	tp->tp_fund != ARRAY || otp->tp_fund != ARRAY ||
			tp->tp_up != otp->tp_up
		)	{
			error("redeclaration of %s with different type",
				idf->id_text);
			return;
		}
		/* Multiple array declaration; this may be interesting */
		if (tp->tp_size < 0)	{	/* new decl has [] */
			/* nothing new */
		}
		else
		if (otp->tp_size < 0)	{	/* old decl has [] */
			def->df_type = tp;
		}
		else
		if (tp->tp_size != otp->tp_size)
			error("inconsistent size in redeclaration of array %s",
				idf->id_text);
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
			IMPLICIT:	function declaration inferred from
					call
	*/
	if (new_sc == IMPLICIT)
		return;			/* no new information */
	switch (def->df_sc)	{	/* the old storage class */
	case EXTERN:
		switch (new_sc)	{	/* the new storage class */
		case EXTERN:
		case GLOBAL:
			break;
		case STATIC:
			if (def->df_initialized)	{
				error("cannot redeclare %s to static",
					idf->id_text);
			}
			else	{
				warning("%s redeclared to static",
						idf->id_text);
			}
			def->df_sc = new_sc;
			break;
		default:
			crash("bad storage class");
			/*NOTREACHED*/
		}
		break;
	case GLOBAL:
		switch (new_sc)	{	/* the new storage class */
		case EXTERN:
			def->df_sc = EXTERN;
			break;
		case GLOBAL:
			break;
		case STATIC:
			if (def->df_initialized)
				error("cannot redeclare %s to static",
					idf->id_text);
			else	{
#ifdef	LINT
				/* warn unconditionally */
				warning("%s redeclared to static",
						idf->id_text);
#else	LINT
#ifndef NOROPTION
				/* warn conditionally */
				if (options['R'])
					warning("%s redeclared to static",
						idf->id_text);
#endif	NOROPTION
#endif	LINT
				def->df_sc = STATIC;
			}
			break;
		default:
			crash("bad storage class");
			/*NOTREACHED*/
		}
		break;
	case STATIC:
		switch (new_sc)	{	/* the new storage class */
		case EXTERN:
			if (def->df_initialized)
				error("cannot redeclare %s to extern",
					idf->id_text);
			else	{
				warning("%s redeclared to extern",
					idf->id_text);
				def->df_sc = EXTERN;
			}
			break;
		case GLOBAL:
		case STATIC:
			if (def->df_type->tp_fund != FUNCTION)
				warning("%s was already static",
					idf->id_text);
			break;
		default:
			crash("bad storage class");
			/*NOTREACHED*/
		}
		break;
	case IMPLICIT:
		switch (new_sc)	{	/* the new storage class */
		case EXTERN:
		case GLOBAL:
			def->df_sc = new_sc;
			break;
		case STATIC:
#ifndef NOROPTION
			if (options['R'])
				warning("%s was implicitly declared as extern",
					idf->id_text);
#endif
			def->df_sc = new_sc;
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
	register struct declarator *dc;
{
	/*	Declares the formal parameters if they exist.
	*/
	register struct formal *fm = dc->dc_formal;

	while (fm)	{
		declare_parameter(fm->fm_idf);
		fm = fm->next;
	}
	free_formals(dc->dc_formal);
	dc->dc_formal = 0;
}

init_idf(idf)
	register struct idf *idf;
{
	/*	The topmost definition of idf is set to initialized.
	*/
	register struct def *def = idf->id_def;	/* the topmost */

	if (def->df_initialized)
		error("multiple initialization of %s", idf->id_text);
	if (def->df_sc == TYPEDEF)	{
		warning("typedef cannot be initialized");
		def->df_sc = EXTERN;		/* ??? *//* What else ? */
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

declare_formals(fp)
	arith *fp;
{
	/*	Declares those formals as int that haven't been declared
		by the user.
		An address is assigned to each formal parameter.
		The total size of the formals is returned in *fp;
	*/
	register struct stack_entry *se = stack_level_of(L_FORMAL1)->sl_entry;
	arith f_offset = (arith)0;

#ifdef	DEBUG
	if (options['t'])
		dumpidftab("start declare_formals", 0);
#endif	DEBUG
	while (se)	{
		register struct def *def = se->se_idf->id_def;

		def->df_address = f_offset;
		/*	the alignment convention for parameters is: align on
			word boundaries, i.e. take care that the following
			parameter starts on a new word boundary.
		*/
		f_offset = align(f_offset + def->df_type->tp_size, (int) word_size);
		formal_cvt(def); /* cvt int to char or short, if necessary */
		def->df_level = L_FORMAL2;	/* CJ */
		RegisterAccount(def->df_address, def->df_type->tp_size,
				regtype(def->df_type),
				def->df_sc);
#ifdef DBSYMTAB
		if (options['g']) {
			stb_string(def, FORMAL, se->se_idf->id_text);
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
#ifndef NOFLOAT
	case FLOAT:
	case DOUBLE:
		return reg_float;
#endif NOFLOAT
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

char hmask[IDFSIZE];

init_hmask()
{
	/*	A simple congruence random number generator, as
		described in Knuth, vol 2.
	*/
	register int h, rnd = HASH_X;

	for (h = 0; h < IDFSIZE; h++)	{
		hmask[h] = rnd;
		rnd = (HASH_A * rnd + HASH_C) & HASHMASK;
	}
}
