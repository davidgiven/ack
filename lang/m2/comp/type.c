/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/*	T Y P E   D E F I N I T I O N   M E C H A N I S M	 */

/* $Header$ */

#include	"debug.h"

#include	<assert.h>
#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>

#include	"nostrict.h"
#include	"LLlex.h"
#include	"def.h"
#include	"type.h"
#include	"idf.h"
#include	"node.h"
#include	"const.h"
#include	"scope.h"
#include	"walk.h"
#include	"chk_expr.h"
#include	"warning.h"

#ifndef NOCROSS
#include	"target_sizes.h"
int
	word_align = AL_WORD,
	short_align = AL_SHORT,
	int_align = AL_INT,
	long_align = AL_LONG,
	float_align = AL_FLOAT,
	double_align = AL_DOUBLE,
	pointer_align = AL_POINTER,
	struct_align = AL_STRUCT;

arith
	word_size = SZ_WORD,
	dword_size = 2 * SZ_WORD,
	int_size = SZ_INT,
	short_size = SZ_SHORT,
	long_size = SZ_LONG,
	float_size = SZ_FLOAT,
	double_size = SZ_DOUBLE,
	pointer_size = SZ_POINTER;
#endif

t_type
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

t_type *
construct_type(fund, tp)
	int fund;
	register t_type *tp;
{
	/*	fund must be a type constructor.
		The pointer to the constructed type is returned.
	*/
	register t_type *dtp = new_type();

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
		dtp->tp_value.tp_arr = new_array();
		if (tp) dtp->tp_align = tp->tp_align;
		break;

	case T_SUBRANGE:
		assert(tp != 0);
		dtp->tp_value.tp_subrange = new_subrange();
		dtp->tp_align = tp->tp_align;
		dtp->tp_size = tp->tp_size;
		break;

	default:
		crash("funny type constructor");
	}

	dtp->tp_next = tp;
	return dtp;
}

arith
align(pos, al)
	arith pos;
	int al;
{
	int i = pos % al;

	if (i) return pos + (al - i);
	return pos;
}

t_type *
standard_type(fund, align, size)
	int fund;
	int align;
	arith size;
{
	register t_type *tp = new_type();

	tp->tp_fund = fund;
	tp->tp_align = align;
	tp->tp_size = size;
	if (fund == T_ENUMERATION || fund == T_CHAR) {
		tp->tp_value.tp_enum = new_enume();
	}

	return tp;
}

InitTypes()
{
	/*	Initialize the predefined types
	*/
	register t_type *tp;

	/* first, do some checking
	*/
	if ((int) int_size != (int) word_size) {
		fatal("integer size not equal to word size");
	}

	if ((int) int_size != (int) pointer_size) {
		fatal("cardinal size not equal to pointer size");
	}

	if ((int) long_size < (int) int_size ||
	    (int) long_size % (int) word_size != 0) {
		fatal("illegal long integer size");
	}

	if ((int) double_size < (int) float_size) {
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
	tp->sub_ub = (int) word_size * 8 - 1;
	bitset_type = set_type(tp);

	/* a unique type for standard procedures and functions
	*/
	std_type = construct_type(T_PROCEDURE, NULLTYPE);

	/* a unique type indicating an error
	*/
	error_type = new_type();
	*error_type = *char_type;
}

STATIC
u_small(tp, n)
	register t_type *tp;
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

t_type *
enum_type(EnumList)
	t_node *EnumList;
{
	register t_type *tp =
		standard_type(T_ENUMERATION, int_align, int_size);

	EnterEnumList(EnumList, tp);
	if (! fit(tp->enm_ncst, (int) int_size)) {
		node_error(EnumList, "too many enumeration literals");
	}
	u_small(tp, (arith) (tp->enm_ncst-1));
	return tp;
}

t_type *
qualified_type(nd)
	register t_node *nd;
{
	register t_def *df;

	if (ChkDesig(nd, D_USED)) {
		if (nd->nd_class != Def) {
			node_error(nd, "type expected");
			FreeNode(nd);
			return error_type;
		}

		df = nd->nd_def;
		if (df->df_kind&(D_ISTYPE|D_FORWARD|D_FORWTYPE|D_ERROR)) {
			if (! df->df_type) {
node_error(nd,"type \"%s\" not (yet) declared", df->df_idf->id_text);
				FreeNode(nd);
				return error_type;
		    	}
			FreeNode(nd);
			if (df->df_kind == D_FORWTYPE) {
				df->df_kind = D_FTYPE;
			}
		   	return df->df_type;
		}
node_error(nd,"identifier \"%s\" is not a type", df->df_idf->id_text);
	}
	FreeNode(nd);
	return error_type;
}

chk_basesubrange(tp, base)
	register t_type *tp, *base;
{
	/*	A subrange had a specified base. Check that the bases conform.
	*/

	assert(tp->tp_fund == T_SUBRANGE);

	if (base->tp_fund == T_SUBRANGE) {
		/* Check that the bounds of "tp" fall within the range
		   of "base".
		*/
		int fund = base->tp_next->tp_fund;

		if (! chk_bounds(base->sub_lb, tp->sub_lb, fund) || 
		    ! chk_bounds(tp->sub_ub, base->sub_ub, fund)) {
			error("base type has insufficient range");
		}
		base = base->tp_next;
	}

	if (base->tp_fund & (T_ENUMERATION|T_CHAR)) {
		if (tp->tp_next != base) {
			error("specified base does not conform");
		}
	}
	else if (base != card_type && base != int_type) {
		error("illegal base for a subrange");
	}
	else if (base == int_type && tp->tp_next == card_type &&
		 (tp->sub_ub > max_int || tp->sub_ub < 0)) {
		error("upperbound to large for type INTEGER");
	}
	else if (base != tp->tp_next && base != int_type) {
		error("specified base does not conform");
	}

	tp->tp_next = base;
}

int
chk_bounds(l1, l2, fund)
	arith l1, l2;
{
	/*	compare to arith's, but be careful. They might be unsigned
	*/
	if (fund == T_INTEGER) {
		return l2 >= l1;
	}
	return (l2 & mach_long_sign ?
		(l1 & mach_long_sign ? l2 >= l1 : 1) :
		(l1 & mach_long_sign ? 0 : l2 >= l1)
	       );
}

t_type *
subr_type(lb, ub)
	register t_node *lb;
	t_node *ub;
{
	/*	Construct a subrange type from the constant expressions
		indicated by "lb" and "ub", but first perform some
		checks
	*/
	register t_type *tp = BaseType(lb->nd_type);
	register t_type *res;

	if (tp == intorcard_type) {
		/* Lower bound >= 0; in this case, the base type is CARDINAL,
		   according to the language definition, par. 6.3
		*/
		assert(lb->nd_INT >= 0);
		tp = card_type;
	}

	if (!ChkCompat(&ub, tp, "subrange bounds")) {
		return error_type;
	}

	/* Check base type
	*/
	if (! (tp->tp_fund & T_DISCRETE)) {
		node_error(lb, "illegal base type for subrange");
		return error_type;
	}

	/* Check bounds
	*/
	if (! chk_bounds(lb->nd_INT, ub->nd_INT, tp->tp_fund)) {
		node_error(lb, "lower bound exceeds upper bound");
	}

	/* Now construct resulting type
	*/
	res = construct_type(T_SUBRANGE, tp);
	res->sub_lb = lb->nd_INT;
	res->sub_ub = ub->nd_INT;
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

t_type *
proc_type(result_type, parameters, n_bytes_params)
	t_type *result_type;
	t_param *parameters;
	arith n_bytes_params;
{
	register t_type *tp = construct_type(T_PROCEDURE, result_type);

	tp->prc_params = parameters;
	tp->prc_nbpar = n_bytes_params;
	if (! fit(n_bytes_params, (int) word_size)) {
		error("maximum parameter byte count exceeded");
	}
	return tp;
}

genrck(tp)
	register t_type *tp;
{
	/*	generate a range check descriptor for type "tp" when
		neccessary. Return its label.
	*/
	arith lb, ub;
	register label ol;
	arith size = tp->tp_size;
	extern char *long2str();
	register t_type *btp = BaseType(tp);

	if (size < word_size) size = word_size;
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
		C_rom_icon(long2str((long)lb,10), size);
		C_rom_icon(long2str((long)ub,10), size);
	}
	c_lae_dlb(ol);
	if (size <= word_size) {
		C_cal(btp->tp_fund == T_INTEGER ? "rcki" : "rcku");
	}
	else {
		C_cal(btp->tp_fund == T_INTEGER ? "rckil" : "rckul");
	}
	C_asp(pointer_size);
}

getbounds(tp, plo, phi)
	register t_type *tp;
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

t_type *
set_type(tp)
	register t_type *tp;
{
	/*	Construct a set type with base type "tp", but first
		perform some checks
	*/
	arith lb, ub, diff;

	if (! bounded(tp)) {
		error("illegal base type for set");
		return error_type;
	}

	getbounds(tp, &lb, &ub);

#ifndef NOSTRICT
	if (lb < 0) {
		warning(W_STRICT, "base type of set has negative lower bound");
	}
#endif

	diff = ub - lb + 1;
	if (diff < 0 || (sizeof(int) == 2 && diff > 65535)) {
		error("set type limits exceeded");
		return error_type;
	}

	tp = construct_type(T_SET, tp);
	tp->tp_size = WA((diff + 7) >> 3);
	tp->set_low = lb;
	return tp;
}

arith
ArrayElSize(tp)
	register t_type *tp;
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
	register t_type *tp;
{
	/*	Assign sizes to an array type, and check index type
	*/
	register t_type *index_type = IndexType(tp);
	register t_type *elem_type = tp->arr_elem;
	arith lo, hi, diff;

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
	diff = hi - lo;

	tp->tp_size = (diff + 1) * tp->arr_elsize;
	if (! fit(tp->tp_size, (int) word_size)) {
		error("array too large");
	}

	/* generate descriptor and remember label.
	*/
	tp->arr_descr = ++data_label;
	C_df_dlb(tp->arr_descr);
	C_rom_cst(lo);
	C_rom_cst(diff);
	C_rom_cst(tp->arr_elsize);
}

FreeType(tp)
	register t_type *tp;
{
	/*	Release type structures indicated by "tp".
		This procedure is only called for types, constructed with
		T_PROCEDURE.
	*/
	register t_param *pr, *pr1;

	assert(tp->tp_fund == T_PROCEDURE);

	pr = ParamList(tp);
	while (pr) {
		pr1 = pr;
		pr = pr->par_next;
		free_def(pr1->par_def);
		free_paramlist(pr1);
	}

	free_type(tp);
}

DeclareType(nd, df, tp)
	register t_def *df;
	register t_type *tp;
	t_node *nd;
{
	/*	A type with type-description "tp" is declared and must
		be bound to definition "df".
		This routine also handles the case that the type-field of
		"df" is already bound. In that case, it is either an opaque
		type, or an error message was given when "df" was created.
	*/
	register t_type *df_tp = df->df_type;

	if (df_tp && df_tp->tp_fund == T_HIDDEN) {
	  	if (! (tp->tp_fund & (T_POINTER|T_HIDDEN|T_EQUAL))) {
			node_error(nd,
				   "opaque type \"%s\" is not a pointer type",
				   df->df_idf->id_text);
		}
		df_tp->tp_next = tp;
		df_tp->tp_fund = T_EQUAL;
		while (tp != df_tp && tp->tp_fund == T_EQUAL) {
			tp = tp->tp_next;
		}
		if (tp == df_tp) {
			/* Circular definition! */
			node_error(nd,
				 "opaque type \"%s\" has a circular definition",
				 df->df_idf->id_text);
		}
	}
	else	df->df_type = tp;
}

t_type *
RemoveEqual(tpx)
	register t_type *tpx;
{

	if (tpx) while (tpx->tp_fund == T_EQUAL) tpx = tpx->tp_next;
	return tpx;
}

int
type_or_forward(ptp)
	t_type **ptp;
{
	/*	POINTER TO IDENTIFIER construction. The IDENTIFIER resides
		in "dot". This routine handles the different cases.
	*/
	register t_node *nd;
	register t_def *df, *df1;

	if ((df1 = lookup(dot.TOK_IDF, CurrentScope, D_IMPORTED, D_USED))) {
		/* Either a Module or a Type, but in both cases defined
		   in this scope, so this is the correct identification
		*/
		if (df1->df_kind == D_FORWTYPE) {
			nd = dot2node(0, NULLNODE, df1->df_forw_node);
			df1->df_forw_node = nd;
			nd->nd_type = *ptp;
		}
		return 1;
	}
	nd = dot2leaf(0);
	if ((df1 = lookfor(nd, CurrVis, 0, D_USED))->df_kind == D_MODULE) {
		/* A Modulename in one of the enclosing scopes.
		   It is not clear from the language definition that
		   it is correct to handle these like this, but
		   existing compilers do it like this, and the
		   alternative is difficult with a lookahead of only
		   one token.
		   ???
		*/
		FreeNode(nd);
		return 1;
	}
	/*	Enter a forward reference into a list belonging to the
		current scope. This is used for POINTER declarations, which
		may have forward references that must howewer be declared in the
		same scope.
	*/
	df = define(nd->nd_IDF, CurrentScope, D_FORWTYPE);

	if (df->df_kind == D_TYPE) {
		(*ptp)->tp_next = df->df_type;
		FreeNode(nd);
		return 0;
	}
	nd->nd_type = *ptp;
	df->df_forw_node = nd;
	if (df1->df_kind == D_TYPE) {
		df->df_type = df1->df_type;
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
	register t_type *tp;
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
		register t_param *par = ParamList(tp);

		print("PROCEDURE");
		if (par) {
			print("(");
			while(par) {
				if (IsVarParam(par)) print("VAR ");
				DumpType(TypeOfParam(par));
				par = par->par_next;
			}
		}
		break;
		}
	case T_ARRAY:
		print("ARRAY");
		print("; element:");
		DumpType(tp->arr_elem);
		print("; index:");
		DumpType(tp->tp_next);
		print(";");
		return;
	case T_STRING:
		print("STRING"); break;
	case T_INTORCARD:
		print("INTORCARD"); break;
	default:
		crash("DumpType");
	}
	if (tp->tp_next && tp->tp_fund != T_POINTER) {
		/* Avoid printing recursive types!
		*/
		print(" next:(");
		DumpType(tp->tp_next);
		print(")");
	}
	print(";");
}
#endif
