/*	T Y P E   D E F I N I T I O N   M E C H A N I S M	 */

static char *RcsId = "$Header$";

#include	<assert.h>
#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	"def_sizes.h"
#include	"Lpars.h"
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
	case PROCEDURE:
	case POINTER:
		dtp->tp_align = ptr_align;
		dtp->tp_size = ptr_size;
		dtp->next = tp;
		break;
	case SET:
		dtp->tp_align = wrd_align;
		dtp->next = tp;
		break;
	case ARRAY:
		dtp->tp_align = tp->tp_align;
		dtp->next = tp;
		break;
	case SUBRANGE:
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

	char_type = standard_type(CHAR, 1, (arith) 1);
	char_type->enm_ncst = 256;
	bool_type = standard_type(ENUMERATION, 1, (arith) 1);
	bool_type->enm_ncst = 2;
	int_type = standard_type(INTEGER, int_align, int_size);
	longint_type = standard_type(LONGINT, lint_align, lint_size);
	card_type = standard_type(CARDINAL, int_align, int_size);
	real_type = standard_type(REAL, real_align, real_size);
	longreal_type = standard_type(LONGREAL, lreal_align, lreal_size);
	word_type = standard_type(WORD, wrd_align, wrd_size);
	intorcard_type = standard_type(INTORCARD, int_align, int_size);
	string_type = standard_type(STRING, 1, (arith) -1);
	address_type = construct_type(POINTER, word_type);
	tp = construct_type(SUBRANGE, int_type);
	tp->sub_lb = 0;
	tp->sub_ub = wrd_size * 8 - 1;
	bitset_type = set_type(tp);
	std_type = construct_type(PROCEDURE, NULLTYPE);
	error_type = standard_type(ERRONEOUS, 1, (arith) 1);
}

int
has_selectors(df)
	register struct def *df;
{

	switch(df->df_kind) {
	case D_MODULE:
		return df->df_value.df_module.mo_scope;
	case D_VARIABLE: {	
		register struct type *tp = df->df_type;

		if (tp->tp_fund == RECORD) {
			return tp->rec_scope;
		}
		break;
		}
	}
	error("no selectors for \"%s\"", df->df_idf->id_text);
	return 0;
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
	if (base->tp_fund == SUBRANGE) {
		/* Check that the bounds of "tp" fall within the range
		   of "base"
		*/
		if (base->sub_lb > tp->sub_lb || base->sub_ub < tp->sub_ub) {
			error("Base type has insufficient range");
		}
		base = base->next;
	}
	if (base->tp_fund == ENUMERATION || base->tp_fund == CHAR) {
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
}

struct type *
subr_type(lb, ub)
	struct node *lb, *ub;
{
	/*	Construct a subrange type from the constant expressions
		indicated by "lb" and "ub", but first perform some
		checks
	*/
	register struct type *tp = lb->nd_type;

	if (!TstCompat(lb->nd_type, ub->nd_type)) {
		node_error(ub, "Types of subrange bounds not compatible");
		return error_type;
	}

	if (tp->tp_fund == SUBRANGE) tp = tp->next;
	if (tp == intorcard_type) tp = card_type;	/* lower bound > 0 */

	/* Check base type
	*/
	if (tp != int_type && tp != card_type && tp != char_type &&
	    tp->tp_fund != ENUMERATION) {
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
	tp = construct_type(SUBRANGE, tp);
	tp->sub_lb = lb->nd_INT;
	tp->sub_ub = ub->nd_INT;
	DO_DEBUG(2,debug("Creating subrange type %ld-%ld", (long)lb->nd_INT,(long)ub->nd_INT));
	return tp;
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

	if (tp->tp_fund == SUBRANGE) {
		if ((lb = tp->sub_lb) < 0 || (ub = tp->sub_ub) > MAX_SET - 1) {
			error("Set type limits exceeded");
			return error_type;
		}
	}
	else if (tp->tp_fund == ENUMERATION || tp == char_type) {
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
	tp = construct_type(SET, tp);
	tp->tp_size = align(((ub - lb) + 7)/8, wrd_align);
	return tp;
}
