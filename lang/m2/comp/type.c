/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/*	T Y P E   D E F I N I T I O N   M E C H A N I S M	 */

/* $Header$ */

#include	"target_sizes.h"
#include	"debug.h"
#include	"maxset.h"

#include	<assert.h>
#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>

#include	"def.h"
#include	"type.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"node.h"
#include	"const.h"
#include	"scope.h"
#include	"walk.h"
#include	"chk_expr.h"

int
	word_align = AL_WORD,
	short_align = AL_SHORT,
	int_align = AL_INT,
	long_align = AL_LONG,
	float_align = AL_FLOAT,
	double_align = AL_DOUBLE,
	pointer_align = AL_POINTER,
	struct_align = AL_STRUCT;

int
	maxset = MAXSET;

arith
	word_size = SZ_WORD,
	dword_size = 2 * SZ_WORD,
	int_size = SZ_INT,
	short_size = SZ_SHORT,
	long_size = SZ_LONG,
	float_size = SZ_FLOAT,
	double_size = SZ_DOUBLE,
	pointer_size = SZ_POINTER;

struct type
	*bool_type,
	*char_type,
	*int_type,
	*card_type,
	*longint_type,
	*real_type,
	*longreal_type,
	*word_type,
	*byte_type,
	*address_type,
	*intorcard_type,
	*bitset_type,
	*std_type,
	*error_type;

struct type *
construct_type(fund, tp)
	int fund;
	register struct type *tp;
{
	/*	fund must be a type constructor.
		The pointer to the constructed type is returned.
	*/
	register struct type *dtp = new_type();

	switch (dtp->tp_fund = fund)	{
	case T_PROCEDURE:
	case T_POINTER:
	case T_HIDDEN:
		dtp->tp_align = pointer_align;
		dtp->tp_size = pointer_size;
		break;

	case T_SET:
		dtp->tp_align = word_align;
		break;

	case T_ARRAY:
		if (tp) dtp->tp_align = tp->tp_align;
		break;

	case T_SUBRANGE:
		assert(tp != 0);
		dtp->tp_align = tp->tp_align;
		dtp->tp_size = tp->tp_size;
		break;

	default:
		crash("funny type constructor");
	}

	dtp->next = tp;
	return dtp;
}

arith
align(pos, al)
	arith pos;
	int al;
{
	arith i = pos % al;

	if (i) return pos + al - i;
	return pos;
}

struct type *
standard_type(fund, align, size)
	int fund;
	int align;
	arith size;
{
	register struct type *tp = new_type();

	if (align == 0) align = 1;

	tp->tp_fund = fund;
	tp->tp_align = align;
	tp->tp_size = size;

	return tp;
}

InitTypes()
{
	/*	Initialize the predefined types
	*/
	register struct type *tp;

	/* first, do some checking
	*/
	if (int_size != word_size) {
		fatal("integer size not equal to word size");
	}

	if (int_size != pointer_size) {
		fatal("cardinal size not equal to pointer size");
	}

	if (long_size < int_size || long_size % word_size != 0) {
		fatal("illegal long integer size");
	}

	if (double_size < float_size) {
		fatal("long real size smaller than real size");
	}

	/* character type
	*/
	char_type = standard_type(T_CHAR, 1, (arith) 1);
	char_type->enm_ncst = 256;
	
	/* boolean type
	*/
	bool_type = standard_type(T_ENUMERATION, 1, (arith) 1);
	bool_type->enm_ncst = 2;

	/* integer types, also a "intorcard", for integer constants between
	   0 and MAX(INTEGER)
	*/
	int_type = standard_type(T_INTEGER, int_align, int_size);
	longint_type = standard_type(T_INTEGER, long_align, long_size);
	card_type = standard_type(T_CARDINAL, int_align, int_size);
	intorcard_type = standard_type(T_INTORCARD, int_align, int_size);

	/* floating types
	*/
	real_type = standard_type(T_REAL, float_align, float_size);
	longreal_type = standard_type(T_REAL, double_align, double_size);

	/* SYSTEM types
	*/
	word_type = standard_type(T_WORD, word_align, word_size);
	byte_type = standard_type(T_WORD, 1, (arith) 1);
	address_type = construct_type(T_POINTER, word_type);

	/* create BITSET type
	   TYPE BITSET = SET OF [0..W-1];
	   The subrange is a subrange of type cardinal, because the lower bound
	   is a non-negative integer (See Rep. 6.3)
	*/
	tp = construct_type(T_SUBRANGE, card_type);
	tp->sub_lb = 0;
	tp->sub_ub = word_size * 8 - 1;
	bitset_type = set_type(tp);

	/* a unique type for standard procedures and functions
	*/
	std_type = construct_type(T_PROCEDURE, NULLTYPE);

	/* a unique type indicating an error
	*/
	error_type = standard_type(T_CHAR, 1, (arith) 1);
}

STATIC
u_small(tp, n)
	register struct type *tp;
	arith n;
{
	if (ufit(n, 1)) {
		tp->tp_size = 1;
		tp->tp_align = 1;
	}
	else if (ufit(n, (int)short_size)) {
		tp->tp_size = short_size;
		tp->tp_align = short_align;
	}
}

struct type *
enum_type(EnumList)
	struct node *EnumList;
{
	register struct type *tp =
		standard_type(T_ENUMERATION, int_align, int_size);

	EnterEnumList(EnumList, tp);
	if (! fit(tp->enm_ncst, (int) int_size)) {
		node_error(EnumList, "too many enumeration literals");
	}
	u_small(tp, (arith) (tp->enm_ncst-1));
	return tp;
}

struct type *
qualified_type(nd)
	struct node *nd;
{
	struct type *tp = error_type;

	if (ChkDesignator(nd)) {
		if (nd->nd_class != Def) node_error(nd, "type expected");
		else {
			register struct def *df = nd->nd_def;

			if (df->df_kind&(D_ISTYPE|D_FORWARD|D_FORWTYPE|D_ERROR)) {
			    if (! df->df_type) {
node_error(nd,"type \"%s\" not (yet) declared", df->df_idf->id_text);
			    }
			    else {
				if (df->df_kind == D_FORWTYPE) {
					df->df_kind = D_FTYPE;
				}
			   	tp = df->df_type;
			    }
			}
			else {
node_error(nd,"identifier \"%s\" is not a type", df->df_idf->id_text);
			}
		}
	}
	FreeNode(nd);
	return tp;
}

chk_basesubrange(tp, base)
	register struct type *tp, *base;
{
	/*	A subrange had a specified base. Check that the bases conform.
	*/

	assert(tp->tp_fund == T_SUBRANGE);

	if (base->tp_fund == T_SUBRANGE) {
		/* Check that the bounds of "tp" fall within the range
		   of "base".
		*/
		if (base->sub_lb > tp->sub_lb || base->sub_ub < tp->sub_ub) {
			error("base type has insufficient range");
		}
		base = base->next;
	}

	if (base->tp_fund & (T_ENUMERATION|T_CHAR)) {
		if (tp->next != base) {
			error("specified base does not conform");
		}
	}
	else if (base != card_type && base != int_type) {
		error("illegal base for a subrange");
	}
	else if (base == int_type && tp->next == card_type &&
		 (tp->sub_ub > max_int || tp->sub_ub < 0)) {
		error("upperbound to large for type INTEGER");
	}
	else if (base != tp->next && base != int_type) {
		error("specified base does not conform");
	}

	tp->next = base;
	tp->tp_size = base->tp_size;
	tp->tp_align = base->tp_align;
}

struct type *
subr_type(lb, ub)
	register struct node *lb;
	struct node *ub;
{
	/*	Construct a subrange type from the constant expressions
		indicated by "lb" and "ub", but first perform some
		checks
	*/
	register struct type *tp = BaseType(lb->nd_type);
	register struct type *res;

	if (!TstCompat(lb->nd_type, ub->nd_type)) {
		node_error(lb, "types of subrange bounds not equal");
		return error_type;
	}

	if (tp == intorcard_type) {
		/* Lower bound >= 0; in this case, the base type is CARDINAL,
		   according to the language definition, par. 6.3
		*/
		assert(lb->nd_INT >= 0);
		tp = card_type;
	}

	/* Check base type
	*/
	if (! (tp->tp_fund & T_DISCRETE)) {
		node_error(lb, "illegal base type for subrange");
		return error_type;
	}

	/* Check bounds
	*/
	if (lb->nd_INT > ub->nd_INT) {
		node_error(lb, "lower bound exceeds upper bound");
	}

	/* Now construct resulting type
	*/
	res = construct_type(T_SUBRANGE, tp);
	res->sub_lb = lb->nd_INT;
	res->sub_ub = ub->nd_INT;
	res->tp_size = tp->tp_size;
	res->tp_align = tp->tp_align;
	if (tp == card_type) {
		u_small(res, res->sub_ub);
	}
	else if (tp == int_type) {
		if (fit(res->sub_lb, 1) && fit(res->sub_ub, 1)) {
			res->tp_size = 1;
			res->tp_align = 1;
		}
		else if (fit(res->sub_lb, (int)short_size) &&
			 fit(res->sub_ub, (int)short_size)) {
			res->tp_size = short_size;
			res->tp_align = short_align;
		}
	}
	return res;
}

struct type *
proc_type(result_type, parameters, n_bytes_params)
	struct type *result_type;
	struct paramlist *parameters;
	arith n_bytes_params;
{
	register struct type *tp = construct_type(T_PROCEDURE, result_type);

	tp->prc_params = parameters;
	tp->prc_nbpar = n_bytes_params;
	return tp;
}

genrck(tp)
	register struct type *tp;
{
	/*	generate a range check descriptor for type "tp" when
		neccessary. Return its label.
	*/
	arith lb, ub;
	register label ol;

	getbounds(tp, &lb, &ub);

	if (tp->tp_fund == T_SUBRANGE) {
		if (!(ol = tp->sub_rck)) {
			tp->sub_rck = ++data_label;
		}
	}
	else if (!(ol = tp->enm_rck)) {
		tp->enm_rck = ++data_label;
	}
	if (!ol) {
		C_df_dlb(ol = data_label);
		C_rom_cst(lb);
		C_rom_cst(ub);
	}
	C_lae_dlb(ol, (arith) 0);
	C_rck(word_size);
}

getbounds(tp, plo, phi)
	register struct type *tp;
	arith *plo, *phi;
{
	/*	Get the bounds of a bounded type
	*/

	assert(bounded(tp));

	if (tp->tp_fund == T_SUBRANGE) {
		*plo = tp->sub_lb;
		*phi = tp->sub_ub;
	}
	else {
		*plo = 0;
		*phi = tp->enm_ncst - 1;
	}
}

struct type *
set_type(tp)
	register struct type *tp;
{
	/*	Construct a set type with base type "tp", but first
		perform some checks
	*/
	arith lb, ub;

	if (! bounded(tp)) {
		error("illegal base type for set");
		return error_type;
	}

	getbounds(tp, &lb, &ub);

	if (lb < 0 || ub > maxset-1) {
		error("set type limits exceeded");
		return error_type;
	}

	tp = construct_type(T_SET, tp);
	tp->tp_size = WA((ub - lb + 8) >> 3);
	return tp;
}

arith
ArrayElSize(tp)
	register struct type *tp;
{
	/* Align element size to alignment requirement of element type.
	   Also make sure that its size is either a dividor of the word_size,
	   or a multiple of it.
	*/
	register arith algn;

	if (tp->tp_fund == T_ARRAY) ArraySizes(tp);
	algn = align(tp->tp_size, tp->tp_align);
	if (word_size % algn != 0) {
		/* algn is not a dividor of the word size, so make sure it
		   is a multiple
		*/
		return WA(algn);
	}
	return algn;
}

ArraySizes(tp)
	register struct type *tp;
{
	/*	Assign sizes to an array type, and check index type
	*/
	register struct type *index_type = IndexType(tp);
	register struct type *elem_type = tp->arr_elem;
	arith lo, hi;

	tp->arr_elsize = ArrayElSize(elem_type);
	tp->tp_align = elem_type->tp_align;

	/* check index type
	*/
	if (! bounded(index_type)) {
		error("illegal index type");
		tp->tp_size = tp->arr_elsize;
		return;
	}

	getbounds(index_type, &lo, &hi);

	tp->tp_size = (hi - lo + 1) * tp->arr_elsize;

	/* generate descriptor and remember label.
	*/
	tp->arr_descr = ++data_label;
	C_df_dlb(tp->arr_descr);
	C_rom_cst(lo);
	C_rom_cst(hi - lo);
	C_rom_cst(tp->arr_elsize);
}

FreeType(tp)
	struct type *tp;
{
	/*	Release type structures indicated by "tp".
		This procedure is only called for types, constructed with
		T_PROCEDURE.
	*/
	register struct paramlist *pr, *pr1;

	assert(tp->tp_fund == T_PROCEDURE);

	pr = ParamList(tp);
	while (pr) {
		pr1 = pr;
		pr = pr->next;
		free_def(pr1->par_def);
		free_paramlist(pr1);
	}

	free_type(tp);
}

DeclareType(nd, df, tp)
	register struct def *df;
	register struct type *tp;
	struct node *nd;
{
	/*	A type with type-description "tp" is declared and must
		be bound to definition "df".
		This routine also handles the case that the type-field of
		"df" is already bound. In that case, it is either an opaque
		type, or an error message was given when "df" was created.
	*/

	if (df->df_type && df->df_type->tp_fund == T_HIDDEN) {
	  	if (! (tp->tp_fund & (T_POINTER|T_HIDDEN|T_EQUAL))) {
			node_error(nd,
				   "opaque type \"%s\" is not a pointer type",
				   df->df_idf->id_text);
		}
		df->df_type->next = tp;
		df->df_type->tp_fund = T_EQUAL;
		while (tp != df->df_type && tp->tp_fund == T_EQUAL) {
			tp = tp->next;
		}
		if (tp == df->df_type) {
			/* Circular definition! */
			node_error(nd,
				 "opaque type \"%s\" has a circular definition",
				 df->df_idf->id_text);
		}
	}
	else	df->df_type = tp;
}

struct type *
RemoveEqual(tpx)
	register struct type *tpx;
{

	if (tpx) while (tpx->tp_fund == T_EQUAL) tpx = tpx->next;
	return tpx;
}

int
type_or_forward(ptp)
	struct type **ptp;
{
	/*	POINTER TO IDENTIFIER construction. The IDENTIFIER resides
		in "dot". This routine handles the different cases.
	*/
	register struct node *nd;
	register struct def *df1;

	*ptp = construct_type(T_POINTER, NULLTYPE);
	if ((df1 = lookup(dot.TOK_IDF, CurrentScope, 1))) {
		/* Either a Module or a Type, but in both cases defined
		   in this scope, so this is the correct identification
		*/
		if (df1->df_kind == D_FORWTYPE) {
			nd = new_node();
			nd->nd_token = dot;
			nd->nd_right = df1->df_forw_node;
			df1->df_forw_node = nd;
			nd->nd_type = *ptp;
		}
		return 1;
	}
	nd = new_node();
	nd->nd_token = dot;
	if ((df1 = lookfor(nd, CurrVis, 0))->df_kind == D_MODULE) {
		/* A Modulename in one of the enclosing scopes.
		   It is not clear from the language definition that
		   it is correct to handle these like this, but
		   existing compilers do it like this, and the
		   alternative is difficult with a lookahead of only
		   one token.
		   ???
		*/
		free_node(nd);
		return 1;
	}
	/*	Enter a forward reference into a list belonging to the
		current scope. This is used for POINTER declarations, which
		may have forward references that must howewer be declared in the
		same scope.
	*/
	{
		register struct def *df =
			define(nd->nd_IDF, CurrentScope, D_FORWTYPE);

		if (df->df_kind == D_TYPE) {
			(*ptp)->next = df->df_type;
			free_node(nd);
		}
		else {
			nd->nd_type = *ptp;
			df->df_forw_node = nd;
			if (df1->df_kind == D_TYPE) {
				df->df_type = df1->df_type;
			}
		}
	}
	return 0;
}

int
gcd(m, n)
	register int m, n;
{
	/*	Greatest Common Divisor
 	*/
	register int r;

	while (n)	{
		r = m % n;
		m = n;
		n = r;
	}
	return m;
}

int
lcm(m, n)
	int m, n;
{
	/*	Least Common Multiple
 	*/
	return m * (n / gcd(m, n));
}

#ifdef DEBUG
DumpType(tp)
	register struct type *tp;
{
	if (!tp) return;

	print("align:%d; size:%ld;", tp->tp_align, (long) tp->tp_size);

	print(" fund:");
	switch(tp->tp_fund) {
	case T_RECORD:
		print("RECORD");
		break;
	case T_ENUMERATION:
		print("ENUMERATION; ncst:%d", tp->enm_ncst); break;
	case T_INTEGER:
		print("INTEGER"); break;
	case T_CARDINAL:
		print("CARDINAL"); break;
	case T_REAL:
		print("REAL"); break;
	case T_HIDDEN:
		print("HIDDEN"); break;
	case T_EQUAL:
		print("EQUAL"); break;
	case T_POINTER:
		print("POINTER"); break;
	case T_CHAR:
		print("CHAR"); break;
	case T_WORD:
		print("WORD"); break;
	case T_SET:
		print("SET"); break;
	case T_SUBRANGE:
		print("SUBRANGE %ld-%ld", (long) tp->sub_lb, (long) tp->sub_ub);
		break;
	case T_PROCEDURE:
		{
		register struct paramlist *par = ParamList(tp);

		print("PROCEDURE");
		if (par) {
			print("(");
			while(par) {
				if (IsVarParam(par)) print("VAR ");
				DumpType(TypeOfParam(par));
				par = par->next;
			}
		}
		break;
		}
	case T_ARRAY:
		print("ARRAY");
		print("; element:");
		DumpType(tp->arr_elem);
		print("; index:");
		DumpType(tp->next);
		print(";");
		return;
	case T_STRING:
		print("STRING"); break;
	case T_INTORCARD:
		print("INTORCARD"); break;
	default:
		crash("DumpType");
	}
	if (tp->next && tp->tp_fund != T_POINTER) {
		/* Avoid printing recursive types!
		*/
		print(" next:(");
		DumpType(tp->next);
		print(")");
	}
	print(";");
}
#endif
