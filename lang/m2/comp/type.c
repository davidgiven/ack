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
#include	"misc.h"

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
	*nil_type,
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
	char_type = standard_type(CHAR, 1, (arith) 1);
	bool_type = standard_type(BOOLEAN, 1, (arith) 1);
	int_type = standard_type(INTEGER, int_align, int_size);
	longint_type = standard_type(LONGINT, lint_align, lint_size);
	card_type = standard_type(CARDINAL, int_align, int_size);
	real_type = standard_type(REAL, real_align, real_size);
	longreal_type = standard_type(LONGREAL, lreal_align, lreal_size);
	nil_type = standard_type(POINTER, ptr_align, ptr_size);
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
			return tp->rec_scopenr;
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
	register struct id_list *ids;
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
	else if (base != tp->next && base != int_type) {
		error("Specified base does not conform");
	}
	tp->next = base;
}
