/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/*	T Y P E   D E F I N I T I O N   M E C H A N I S M	 */

/* $Id$ */

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
#include	"scope.h"
#include	"walk.h"
#include	"main.h"
#include	"chk_expr.h"
#include	"warning.h"
#include	"uns_arith.h"

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

#define arith_sign	((arith) (1L << (sizeof(arith) * 8 - 1)))

arith	ret_area_size;

t_type
	*bool_type,
	*char_type,
	*int_type,
	*card_type,
	*longint_type,
	*longcard_type,
	*real_type,
	*longreal_type,
	*word_type,
	*byte_type,
	*address_type,
	*intorcard_type,
	*bitset_type,
	*void_type,
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
		dtp->tp_align = struct_align;
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
standard_type(fund, algn, size)
	int fund;
	int algn;
	arith size;
{
	register t_type *tp = new_type();

	tp->tp_fund = fund;
	tp->tp_align = algn;
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

	if ((int) long_size < (int) int_size) {
		fatal("long integer size smaller than integer size");
	}

	if ((int) double_size < (int) float_size) {
		fatal("long real size smaller than real size");
	}

	ret_area_size = (int) double_size > ((int) pointer_size << 1) ?
				double_size : (pointer_size << 1);

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
	longcard_type = standard_type(T_CARDINAL, long_align, long_size);
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
	void_type = error_type;
}

int
fit(sz, nbytes)
	arith sz;
{
	return ((sz) + ((arith)0x80<<(((nbytes)-1)*8)) & ~full_mask[(nbytes)]) == 0;
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
qualified_type(pnd)
	t_node **pnd;
{
	register t_def *df;

	if (ChkDesig(pnd, D_USED)) {
		register t_node *nd = *pnd;
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
				/*	Here, df->df_type was already set,
					so there is an actual definition in the
					surrounding scope, which is now used.
				*/
				ForceForwardTypeDef(df);
			}
		   	return df->df_type;
		}
node_error(nd, "identifier \"%s\" is not a type", df->df_idf->id_text);
	}
	FreeNode(*pnd);
	return error_type;
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
#ifdef UNSIGNED_ARITH
	return (UNSIGNED_ARITH) l2 >= (UNSIGNED_ARITH) l1;
#else
	return (l2 & arith_sign ?
		(l1 & arith_sign ? l2 >= l1 : 1) :
		(l1 & arith_sign ? 0 : l2 >= l1)
	       );
#endif
}

int
in_range(i, tp)
	arith		i;
	register t_type	*tp;
{
	/*	Check that the value i fits in the subrange or enumeration
		type tp.  Return 1 if so, 0 otherwise
	*/

	switch(tp->tp_fund) {
	case T_ENUMERATION:
	case T_CHAR:
		return i >= 0 && i < tp->enm_ncst;

	case T_SUBRANGE:
		return	chk_bounds(i, tp->sub_ub, SubBaseType(tp)->tp_fund) &&
			chk_bounds(tp->sub_lb, i, SubBaseType(tp)->tp_fund);
	}
	assert(0);
	/*NOTREACHED*/
}

t_type *
subr_type(lb, ub, base)
	register t_node *lb;
	t_node *ub;
	t_type *base;
{
	/*	Construct a subrange type from the constant expressions
		indicated by "lb" and "ub", but first perform some
		checks. "base" is either a user-specified base-type, or NULL.
	*/
	register t_type *tp = BaseType(lb->nd_type);
	register t_type *res;

	if (tp == intorcard_type) {
		/* Lower bound >= 0; in this case, the base type is CARDINAL,
		   according to the language definition, par. 6.3.
		   But what if the upper-bound is of type INTEGER (f.i.
		   MAX(INTEGER)? The Report does not answer this. Fix this
		   for the time being, by making it an INTEGER subrange.
		   ???
		*/
		assert(lb->nd_INT >= 0);
		if (BaseType(ub->nd_type) == int_type ||
		    (base && BaseType(base) == int_type)) tp = int_type;
		else tp = card_type;
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

	/* Now construct resulting type
	*/
	res = construct_type(T_SUBRANGE, tp);
	res->sub_lb = lb->nd_INT;
	res->sub_ub = ub->nd_INT;

	/* Check bounds
	*/
	if (! chk_bounds(lb->nd_INT, ub->nd_INT, tp->tp_fund)) {
		node_error(lb, "lower bound exceeds upper bound");
		ub->nd_INT = lb->nd_INT;
		res->sub_ub = res->sub_lb;
	}

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

	if (base) {
		if (base->tp_fund == T_SUBRANGE) {
			/* Check that the bounds of "res" fall within the range
			   of "base".
			*/
			if (! in_range(res->sub_lb, base) || 
			    ! in_range(res->sub_ub, base)) {
				error("base type has insufficient range");
			}
			base = base->tp_next;
		}
		if ((base->tp_fund & (T_ENUMERATION|T_CHAR)) ||
		    base == card_type) {
			if (res->tp_next != base) {
				error("specified basetype for subrange not compatible with bounds");
			}
		}
		else if (base == int_type) {
			if (res->tp_next == card_type &&
			    ! chk_bounds(res->sub_ub,
					 max_int[(int)int_size],
					 T_CARDINAL)){
				error("upperbound too large for type INTEGER");
			}
		}
		else	error("illegal base for a subrange");
		res->tp_next = base;
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
	if (result_type && ! fit(WA(result_type->tp_size), (int) word_size)) {
		error("maximum return value size exceeded");
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
		CAL(btp->tp_fund == T_INTEGER ? "rcki" : "rcku", (int) pointer_size);
	}
	else {
		CAL(btp->tp_fund == T_INTEGER ? "rckil" : "rckul", (int) pointer_size);
	}
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
	arith lb, ub, diff, alloc_size;

	if (! bounded(tp) || tp->tp_size > word_size) {
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
	if (diff < 0) {
		error("set type limits exceeded");
		return error_type;
	}

	tp = construct_type(T_SET, tp);
	tp->tp_size = WA((diff + 7) >> 3);
	alloc_size = (tp->tp_size / word_size + 1) * sizeof(arith);
	tp->set_sz = alloc_size;
	if (tp->set_sz != alloc_size) {
		error("set size too large");
		return error_type;
	}
	tp->set_low = lb;
	return tp;
}

ArrayElSize(tp)
	register t_type *tp;
{
	/* Align element size to alignment requirement of element type.
	   Also make sure that its size is either a dividor of the word_size,
	   or a multiple of it.
	*/
	register arith algn;
	register t_type *elem_type = tp->arr_elem;

	if (elem_type->tp_fund == T_ARRAY) ArraySizes(elem_type);
	algn = align(elem_type->tp_size, elem_type->tp_align);
	if (word_size % algn != 0) {
		/* algn is not a dividor of the word size, so make sure it
		   is a multiple
		*/
		algn = WA(algn);
	}
	if (! fit(algn, (int) word_size)) {
		error("element size of array too large");
	}
	tp->arr_elsize = algn;
	if (tp->tp_align < elem_type->tp_align) {
		tp->tp_align = elem_type->tp_align;
	}
}

ArraySizes(tp)
	register t_type *tp;
{
	/*	Assign sizes to an array type, and check index type
	*/
	register t_type *index_type = IndexType(tp);
	arith diff;

	ArrayElSize(tp);

	/* check index type
	*/
	if (index_type->tp_size > word_size || ! bounded(index_type)) {
		error("illegal index type");
		tp->tp_size = tp->arr_elsize;
		return;
	}

	getbounds(index_type, &(tp->arr_low), &(tp->arr_high));
	diff = tp->arr_high - tp->arr_low;

	if (diff < 0 || ! fit(diff, (int) int_size)) {
		error("too many elements in array");
	}

	tp->tp_size = align((diff + 1) * tp->arr_elsize, tp->tp_align);
	/* ??? check overflow ??? */
	if (! ufit(tp->tp_size, (int) pointer_size)) {
		error("array too large");
	}

	/* generate descriptor and remember label.
	*/
	tp->arr_descr = ++data_label;
	C_df_dlb(tp->arr_descr);
	C_rom_cst((arith) 0);
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
			tp->tp_next = error_type;
		}
	}
	else {
		df->df_type = tp;
		if (BaseType(tp)->tp_fund == T_ENUMERATION) {
			CheckForImports(df);
		}
	}
#ifdef DBSYMTAB
	if (options['g']) stb_string(df, D_TYPE);
#endif

	SolveForwardTypeRefs(df);
}

SolveForwardTypeRefs(df)
	register t_def *df;
{
	register t_node *nd;

	if (df->df_kind == D_FORWTYPE) {
		nd = df->df_forw_node;

		df->df_kind = D_TYPE;
		while (nd) {
			nd->nd_type->tp_next = df->df_type;
#ifdef DBSYMTAB
			if (options['g'] && nd->nd_type->tp_dbindex < 0) {
				stb_addtp("(forward_type)", nd->nd_type);
			}
#endif
			nd = nd->nd_RIGHT;
		}
		FreeNode(df->df_forw_node);
	}
}


ForceForwardTypeDef(df)
	register t_def *df;
{
	register t_def *df1 = df, *df2;
	register t_node *nd = df->df_forw_node;

	while (df && df->df_kind == D_FORWTYPE) {
		RemoveFromIdList(df);
		if ((df2 = df->df_scope->sc_def) == df) {
			df->df_scope->sc_def = df->df_nextinscope;
		}
		else {
			while (df2->df_nextinscope != df) {
				df2 = df2->df_nextinscope;
			}
			df2->df_nextinscope = df->df_nextinscope;
		}
		df = df->df_forw_def;
	}
	while (nd->nd_class == Link) {
		nd = nd->nd_RIGHT;
	}
	df = lookfor(nd, CurrVis, 1, 0);
	if (! df->df_kind & (D_ERROR|D_TYPE)) {
		node_error(nd, "\"%s\" is not a type", df1->df_idf->id_text);
	}
	while (df1 && df1->df_kind == D_FORWTYPE) {
		df2 = df1->df_forw_def;
		df1->df_type = df->df_type;
		SolveForwardTypeRefs(df1);
		free_def(df1);
		df1 = df2;
	}
}

t_type *
RemoveEqual(tpx)
	register t_type *tpx;
{

	if (tpx) while (tpx->tp_fund == T_EQUAL) tpx = tpx->tp_next;
	return tpx;
}

int
type_or_forward(tp)
	t_type *tp;
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
		switch(df1->df_kind) {
		case D_FORWARD:
			FreeNode(df1->for_node);
			df1->df_kind = D_FORWTYPE;
			df1->df_forw_node = 0;
			/* Fall through */
		case D_FORWTYPE:
			nd = dot2node(Link, NULLNODE, df1->df_forw_node);
			df1->df_forw_node = nd;
			nd->nd_type = tp;
			return 0;
		default:
			return 1;
		}
	}
	nd = dot2leaf(Name);
	if ((df1 = lookfor(nd, CurrVis, 0, D_USED))->df_kind == D_MODULE) {
		/* A Modulename in one of the enclosing scopes.
		   It is not clear from the language definition that
		   it is correct to handle these like this, but
		   existing compilers do it like this, and the
		   alternative is difficult with a lookahead of only
		   one token.
		   This path should actually only be taken if the next token
		   is a '.'.
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
	assert(df->df_kind == D_FORWTYPE);
	df->df_flags |= D_USED | D_DEFINED;
	nd->nd_type = tp;
	df->df_forw_node = nd;
	if (df != df1 && (df1->df_kind & (D_TYPE | D_FORWTYPE))) {
		/*	"df1" refers to a possible identification, but
			we cannot be sure at this point. For the time
			being, however, we use this one.
		*/
		df->df_type = df1->df_type;
		df->df_forw_def = df1;
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

t_type *
intorcard(left, right)
	register t_type *left, *right;
{
	if (left == intorcard_type) {
		t_type *tmp = left;
		left = right;
		right = tmp;
	}
	if (right == intorcard_type) {
		if (left == int_type || left == card_type) {
			return left;
		}
	}
	return 0;
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
