/*	T Y P E   D E F I N I T I O N   M E C H A N I S M	 */

static char *RcsId = "$Header$";

#include	<assert.h>
#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	"def_sizes.h"
#include	"def.h"
#include	"type.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"node.h"
#include	"const.h"
#include	"debug.h"

/*	To be created dynamically in main() from defaults or from command
	line parameters.
*/
int
	wrd_align = AL_WORD,
	int_align = AL_INT,
	lint_align = AL_LONG,
	real_align = AL_FLOAT,
	lreal_align = AL_DOUBLE,
	ptr_align = AL_POINTER,
	record_align = AL_STRUCT;

arith
	wrd_size = SZ_WORD,
	int_size = SZ_INT,
	lint_size = SZ_LONG,
	real_size = SZ_FLOAT,
	lreal_size = SZ_DOUBLE,
	ptr_size = SZ_POINTER;

struct type
	*bool_type,
	*char_type,
	*int_type,
	*card_type,
	*longint_type,
	*real_type,
	*longreal_type,
	*word_type,
	*address_type,
	*intorcard_type,
	*string_type,
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
		dtp->tp_align = ptr_align;
		dtp->tp_size = ptr_size;
		dtp->next = tp;
		break;
	case T_SET:
		dtp->tp_align = wrd_align;
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
	register struct type *tp;

	char_type = standard_type(T_CHAR, 1, (arith) 1);
	char_type->enm_ncst = 256;
	bool_type = standard_type(T_ENUMERATION, 1, (arith) 1);
	bool_type->enm_ncst = 2;
	int_type = standard_type(T_INTEGER, int_align, int_size);
	longint_type = standard_type(T_INTEGER, lint_align, lint_size);
	card_type = standard_type(T_CARDINAL, int_align, int_size);
	real_type = standard_type(T_REAL, real_align, real_size);
	longreal_type = standard_type(T_REAL, lreal_align, lreal_size);
	word_type = standard_type(T_WORD, wrd_align, wrd_size);
	intorcard_type = standard_type(T_INTORCARD, int_align, int_size);
	string_type = standard_type(T_STRING, 1, (arith) -1);
	address_type = construct_type(T_POINTER, word_type);
	tp = construct_type(T_SUBRANGE, int_type);
	tp->sub_lb = 0;
	tp->sub_ub = wrd_size * 8 - 1;
	bitset_type = set_type(tp);
	std_type = construct_type(T_PROCEDURE, NULLTYPE);
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

/*	A subrange had a specified base. Check that the bases conform ...
*/
chk_basesubrange(tp, base)
	register struct type *tp, *base;
{
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
	if (tp == intorcard_type) tp = card_type;	/* lower bound > 0 */

	/* Check base type
	*/
	if (tp != int_type && tp != card_type && tp != char_type &&
	    tp->tp_fund != T_ENUMERATION) {
		/* BOOLEAN is also an ENUMERATION type
		*/
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
	DO_DEBUG(2,debug("Creating subrange type %ld-%ld", (long)lb->nd_INT,(long)ub->nd_INT));
	return res;
}
#define MAX_SET	1024	/* ??? Maximum number of elements in a set */

struct type *
set_type(tp)
	struct type *tp;
{
	/*	Construct a set type with base type "tp", but first
		perform some checks
	*/
	int lb, ub;

	if (tp->tp_fund == T_SUBRANGE) {
		if ((lb = tp->sub_lb) < 0 || (ub = tp->sub_ub) > MAX_SET - 1) {
			error("Set type limits exceeded");
			return error_type;
		}
	}
	else if (tp->tp_fund == T_ENUMERATION || tp == char_type) {
		lb = 0;
		if ((ub = tp->enm_ncst - 1) > MAX_SET - 1) {
			error("Set type limits exceeded");
			return error_type;
		}
	}
	else {
		error("illegal base type for set");
		return error_type;
	}
	tp = construct_type(T_SET, tp);
	tp->tp_size = align(((ub - lb) + 7)/8, wrd_align);
	return tp;
}

ArraySizes(tp)
	register struct type *tp;
{
	/*	Assign sizes to an array type
	*/
	arith elem_size;
	register struct type *itype = tp->next;	/* the index type */

	if (tp->arr_elem->tp_fund == T_ARRAY) {
		ArraySizes(tp->arr_elem);
	}

	elem_size = align(tp->arr_elem->tp_size, tp->arr_elem->tp_align);
	tp->tp_align = tp->arr_elem->tp_align;

	if (! (itype->tp_fund & T_INDEX)) {
		error("Illegal index type");
		tp->tp_size = 0;
		return;
	}

	switch(itype->tp_fund) {
	case T_SUBRANGE:
		tp->arr_lb = itype->sub_lb;
		tp->arr_ub = itype->sub_ub;
		tp->tp_size = elem_size * (itype->sub_ub - itype->sub_lb + 1);
		break;
	case T_CHAR:
	case T_ENUMERATION:
		tp->arr_lb = 0;
		tp->arr_ub = itype->enm_ncst - 1;
		tp->tp_size = elem_size * itype->enm_ncst;
		break;
	default:
		assert(0);
	}
	/* ??? overflow checking ??? */
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
	register int m, n;
{
	/*	Least Common Multiple
 	*/
	while (m != n) {
		if (m < n) m = m + m;
		else n = n + n;
	}
	return n;		/* or m */
}
