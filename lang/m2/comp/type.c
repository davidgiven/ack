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
construct_type(fund, tp, count)
	struct type *tp;
	arith count;
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
		dtp->tp_size = align((count + 7) / 8, wrd_align);
		dtp->next = tp;
		break;
	case ARRAY:
		dtp->tp_align = tp->tp_align;
		if (tp->tp_size < 0) dtp->tp_size = -1;
		else dtp->tp_size = count * tp->tp_size;
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
