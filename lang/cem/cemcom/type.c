/* $Header$ */
/*	T Y P E   D E F I N I T I O N   M E C H A N I S M	 */

#include	"nobitfield.h"
#include	"alloc.h"
#include	"Lpars.h"
#include	"arith.h"
#include	"type.h"
#include	"idf.h"
#include	"def.h"
#include	"sizes.h"
#include	"align.h"

struct type *function_of(), *array_of();
#ifndef NOBITFIELD
struct type *field_of();
#endif NOBITFIELD

/*	To be created dynamically in main() from defaults or from command
	line parameters.
*/
struct type
	*char_type, *uchar_type,
	*short_type, *ushort_type,
	*word_type, *uword_type,
	*int_type, *uint_type,
	*long_type, *ulong_type,
	*float_type, *double_type,
	*void_type, *label_type,
	*string_type, *funint_type, *error_type;

struct type *pa_type;	/* Pointer-Arithmetic type	*/

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
	arith count; /* for fund == ARRAY only */
{
	/*	fund must be a type constructor: FIELD, FUNCTION, POINTER or
		ARRAY. The pointer to the constructed type is returned.
	*/
	struct type *dtp;

	switch (fund)	{
#ifndef NOBITFIELD
	case FIELD:
		dtp = field_of(tp);
		break;
#endif NOBITFIELD

	case FUNCTION:
		if (tp->tp_fund == FUNCTION)	{
			error("function cannot yield function");
			return error_type;
		}
		if (tp->tp_fund == ARRAY)	{
			error("function cannot yield array");
			return error_type;
		}

		dtp = function_of(tp);
		break;
	case POINTER:
		dtp = pointer_to(tp);
		break;
	case ARRAY:
		if (tp->tp_size < 0)	{
			error("cannot construct array of unknown type");
			count = (arith)-1;
		}
		if (count >= (arith)0)
			count *= tp->tp_size;
		dtp = array_of(tp, count);
		break;
	}
	return dtp;
}

struct type *
function_of(tp)
	struct type *tp;
{
	struct type *dtp = tp->tp_function;

	if (!dtp)	{
		tp->tp_function = dtp = create_type(FUNCTION);
		dtp->tp_up = tp;
		dtp->tp_size = pointer_size;
		dtp->tp_align = pointer_align;
	}
	return dtp;
}

struct type *
pointer_to(tp)
	struct type *tp;
{
	struct type *dtp = tp->tp_pointer;

	if (!dtp)	{
		tp->tp_pointer = dtp = create_type(POINTER);
		dtp->tp_unsigned = 1;
		dtp->tp_up = tp;
		dtp->tp_size = pointer_size;
		dtp->tp_align = pointer_align;
	}
	return dtp;
}

struct type *
array_of(tp, count)
	struct type *tp;
	arith count;
{
	struct type *dtp = tp->tp_array;

	/* look for a type with the right size */
	while (dtp && dtp->tp_size != count)
		dtp = dtp->next;

	if (!dtp)	{
		dtp = create_type(ARRAY);
		dtp->tp_up = tp;
		dtp->tp_size = count;
		dtp->tp_align = tp->tp_align;
		dtp->next = tp->tp_array;
		tp->tp_array = dtp;
	}
	return dtp;
}

#ifndef NOBITFIELD
struct type *
field_of(tp)
	struct type *tp;
{
	struct type *dtp = create_type(FIELD);

	dtp->tp_up = tp;
	dtp->tp_align = tp->tp_align;
	dtp->tp_size = tp->tp_size;
	return dtp;
}
#endif NOBITFIELD

arith
size_of_type(tp, nm)
	struct type *tp;
	char nm[];
{
	arith sz = tp->tp_size;

	if (sz < 0)	{
		error("size of %s unknown", nm);
		return (arith)1;
	}
	return sz;
}

idf2type(idf, tpp)
	struct idf *idf;
	struct type **tpp;
{
	/*	Decoding  a typedef-ed identifier: if the size is yet
		unknown we have to make copy of the type descriptor to
		prevent garbage at the initialisation of arrays with
		unknown size.
	*/
	if (	idf->id_def->df_type->tp_size < (arith)0 &&
		idf->id_def->df_type->tp_fund == ARRAY
	)	{
		struct type *ntp = new_type();
		*ntp = *(idf->id_def->df_type);
			/* this is really a structure assignment, AAGH!!! */
		*tpp = ntp;
	}
	else	{
		*tpp = idf->id_def->df_type;
	}
}

arith
align(pos, al)
	arith pos;
	int al;
{
	return ((pos + al - 1) / al) * al;
}

struct type *
standard_type(fund, sign, align, size)
	int align; arith size;
{
	register struct type *tp = create_type(fund);

	tp->tp_unsigned = sign;
	tp->tp_align = align;
	tp->tp_size = size;

	return tp;
}
