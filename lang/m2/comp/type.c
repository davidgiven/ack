/*	T Y P E   D E F I N I T I O N   M E C H A N I S M	 */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

#include	"target_sizes.h"
#include	"debug.h"
#include	"maxset.h"

#include	<assert.h>
#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"def.h"
#include	"type.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"node.h"
#include	"const.h"

/*	To be created dynamically in main() from defaults or from command
	line parameters.
*/
int
	word_align = AL_WORD,
	int_align = AL_INT,
	long_align = AL_LONG,
	float_align = AL_FLOAT,
	double_align = AL_DOUBLE,
	pointer_align = AL_POINTER,
	struct_align = AL_STRUCT;

arith
	word_size = SZ_WORD,
	int_size = SZ_INT,
	long_size = SZ_LONG,
	float_size = SZ_FLOAT,
	double_size = SZ_DOUBLE,
	pointer_size = SZ_POINTER;

struct type
	*bool_type,
	*char_type,
	*charc_type,
	*int_type,
	*card_type,
	*longint_type,
	*real_type,
	*longreal_type,
	*word_type,
	*address_type,
	*intorcard_type,
	*bitset_type,
	*std_type,
	*error_type;

struct paramlist *h_paramlist;

struct type *h_type;

struct type *
create_type(fund)
	register int fund;
{
	/*	A brand new struct type is created, and its tp_fund set
		to fund.
	*/
	register struct type *ntp = new_type();

	clear((char *)ntp, sizeof(struct type));
	ntp->tp_fund = fund;
	ntp->tp_size = (arith)-1;

	return ntp;
}

struct type *
construct_type(fund, tp)
	struct type *tp;
{
	/*	fund must be a type constructor.
		The pointer to the constructed type is returned.
	*/
	struct type *dtp = create_type(fund);

	switch (fund)	{
	case T_PROCEDURE:
	case T_POINTER:
		dtp->tp_align = pointer_align;
		dtp->tp_size = pointer_size;
		dtp->next = tp;
		if (fund == T_PROCEDURE && tp) {
			if (tp != bitset_type &&
			    !(tp->tp_fund&(T_NUMERIC|T_INDEX|T_WORD|T_POINTER))) {
				error("illegal procedure result type");
			}
		}
		break;

	case T_SET:
		dtp->tp_align = word_align;
		dtp->next = tp;
		break;

	case T_ARRAY:
		dtp->tp_align = tp->tp_align;
		dtp->next = tp;
		break;

	case T_SUBRANGE:
		dtp->tp_align = tp->tp_align;
		dtp->tp_size = tp->tp_size;
		dtp->next = tp;
		break;

	default:
		assert(0);
	}

	return dtp;
}

arith
align(pos, al)
	arith pos;
	int al;
{
	return ((pos + al - 1) / al) * al;
}

struct type *
standard_type(fund, align, size)
	int align; arith size;
{
	register struct type *tp = create_type(fund);

	tp->tp_align = align;
	tp->tp_size = size;

	return tp;
}

init_types()
{
	/*	Initialize the predefined types
	*/
	register struct type *tp;

	/* character type
	*/
	char_type = standard_type(T_CHAR, 1, (arith) 1);
	char_type->enm_ncst = 256;
	
	/* character constant type, different from character type because
	   of compatibility with character array's
	*/
	charc_type = standard_type(T_CHAR, 1, (arith) 1);
	charc_type->enm_ncst = 256;

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
	address_type = construct_type(T_POINTER, word_type);

	/* create BITSET type
	*/
	tp = construct_type(T_SUBRANGE, int_type);
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

/*	Create a parameterlist of a procedure and return a pointer to it.
	"ids" indicates the list of identifiers, "tp" their type, and
	"VARp" is set when the parameters are VAR-parameters.
	Actually, "ids" is only used because it tells us how many parameters
	there were with this type.
*/
struct paramlist *
ParamList(ids, tp, VARp)
	register struct node *ids;
	struct type *tp;
{
	register struct paramlist *pr;
	struct paramlist *pstart;

	pstart = pr = new_paramlist();
	pr->par_type = tp;
	pr->par_var = VARp;
	for (ids = ids->next; ids; ids = ids->next) {
		pr->next = new_paramlist();
		pr = pr->next;
		pr->par_type = tp;
		pr->par_var = VARp;
	}
	pr->next = 0;
	return pstart;
}

chk_basesubrange(tp, base)
	register struct type *tp, *base;
{
	/*	A subrange had a specified base. Check that the bases conform.
	*/

	if (base->tp_fund == T_SUBRANGE) {
		/* Check that the bounds of "tp" fall within the range
		   of "base"
		*/
		if (base->sub_lb > tp->sub_lb || base->sub_ub < tp->sub_ub) {
			error("Base type has insufficient range");
		}
		base = base->next;
	}

	if (base->tp_fund == T_ENUMERATION || base->tp_fund == T_CHAR) {
		if (tp->next != base) {
			error("Specified base does not conform");
		}
	}
	else if (base != card_type && base != int_type) {
		error("Illegal base for a subrange");
	}
	else if (base == int_type && tp->next == card_type &&
		 (tp->sub_ub > max_int || tp->sub_ub)) {
		error("Upperbound to large for type INTEGER");
	}
	else if (base != tp->next && base != int_type) {
		error("Specified base does not conform");
	}

	tp->next = base;
	tp->tp_size = base->tp_size;
	tp->tp_align = base->tp_align;
}

struct type *
subr_type(lb, ub)
	struct node *lb, *ub;
{
	/*	Construct a subrange type from the constant expressions
		indicated by "lb" and "ub", but first perform some
		checks
	*/
	register struct type *tp = lb->nd_type, *res;

	if (!TstCompat(lb->nd_type, ub->nd_type)) {
		node_error(ub, "Types of subrange bounds not compatible");
		return error_type;
	}

	if (tp->tp_fund == T_SUBRANGE) tp = tp->next;

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
		node_error(ub, "Illegal base type for subrange");
		return error_type;
	}

	/* Check bounds
	*/
	if (lb->nd_INT > ub->nd_INT) {
		node_error(ub, "Lower bound exceeds upper bound");
	}

	/* Now construct resulting type
	*/
	res = construct_type(T_SUBRANGE, tp);
	res->sub_lb = lb->nd_INT;
	res->sub_ub = ub->nd_INT;
	res->tp_size = tp->tp_size;
	res->tp_align = tp->tp_align;
	return res;
}

struct type *
set_type(tp)
	struct type *tp;
{
	/*	Construct a set type with base type "tp", but first
		perform some checks
	*/
	arith lb, ub;

	if (tp->tp_fund == T_SUBRANGE) {
		if ((lb = tp->sub_lb) < 0 || (ub = tp->sub_ub) > MAXSET - 1) {
			error("Set type limits exceeded");
			return error_type;
		}
	}
	else if (tp->tp_fund == T_ENUMERATION || tp == char_type) {
		lb = 0;
		if ((ub = tp->enm_ncst - 1) > MAXSET - 1) {
			error("Set type limits exceeded");
			return error_type;
		}
	}
	else {
		error("illegal base type for set");
		return error_type;
	}

	tp = construct_type(T_SET, tp);
	tp->tp_size = align(((ub - lb) + 7)/8, word_align);
	return tp;
}

ArraySizes(tp)
	register struct type *tp;
{
	/*	Assign sizes to an array type, and check index type
	*/
	arith elem_size;
	register struct type *index_type = tp->next;
	register struct type *elem_type = tp->arr_elem;

	if (elem_type->tp_fund == T_ARRAY) {
		ArraySizes(elem_type);
	}

	/* align element size to alignment requirement of element type
	*/
	elem_size = align(elem_type->tp_size, elem_type->tp_align);
	tp->tp_align = elem_type->tp_align;

	/* check index type
	*/
	if (! (index_type->tp_fund & T_INDEX)) {
		error("Illegal index type");
		tp->tp_size = 0;
		return;
	}

	/* find out HIGH, LOW and size of ARRAY
	*/
	switch(index_type->tp_fund) {
	case T_SUBRANGE:
		tp->arr_lb = index_type->sub_lb;
		tp->arr_ub = index_type->sub_ub;
		tp->tp_size = elem_size *
			(index_type->sub_ub - index_type->sub_lb + 1);
		break;
	case T_CHAR:
	case T_ENUMERATION:
		tp->arr_lb = 0;
		tp->arr_ub = index_type->enm_ncst - 1;
		tp->tp_size = elem_size * index_type->enm_ncst;
		break;
	default:
		assert(0);
	}
	/* ??? overflow checking ???
	*/
}

FreeType(tp)
	struct type *tp;
{
	/*	Release type structures indicated by "tp"
	*/
	register struct paramlist *pr, *pr1;

	assert(tp->tp_fund == T_PROCEDURE);

	pr = tp->prc_params;
	while (pr) {
		pr1 = pr;
		pr = pr->next;
		free_paramlist(pr1);
	}

	free_type(tp);
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
	print(" a:%d; s:%ld;", tp->tp_align, (long) tp->tp_size);
	if (tp->next && tp->tp_fund != T_POINTER) {
		/* Avoid printing recursive types!
		*/
		print(" n:(");
		DumpType(tp->next);
		print(")");
	}

	print(" f:");
	switch(tp->tp_fund) {
	case T_RECORD:
		print("RECORD"); break;
	case T_ENUMERATION:
		print("ENUMERATION; n:%d", tp->enm_ncst); break;
	case T_INTEGER:
		print("INTEGER"); break;
	case T_CARDINAL:
		print("CARDINAL"); break;
	case T_REAL:
		print("REAL"); break;
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
		register struct paramlist *par = tp->prc_params;

		print("PROCEDURE");
		if (par) {
			print("; p:");
			while(par) {
				if (par->par_var) print("VAR ");
				DumpType(par->par_type);
				par = par->next;
			}
		}
		break;
		}
	case T_ARRAY:
		print("ARRAY %ld-%ld", (long) tp->arr_lb, (long) tp->arr_ub);
		print("; el:");
		DumpType(tp->arr_elem);
		break;
	case T_STRING:
		print("STRING"); break;
	case T_INTORCARD:
		print("INTORCARD"); break;
	default:
		assert(0);
	}
	print(";");
}
#endif
