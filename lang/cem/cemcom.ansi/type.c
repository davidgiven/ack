/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	T Y P E   D E F I N I T I O N   M E C H A N I S M	 */

#include	"parameters.h"
#include	<alloc.h>
#include    "idf.h"
#include	"Lpars.h"
#include	"arith.h"
#include	"type.h"
#include	"def.h"
#include	"proto.h"
#include	"sizes.h"
#include	"align.h"
#include	"decspecs.h"
#include    "error.h"



/*	To be created dynamically in main() from defaults or from command
 line parameters.
 */
struct type *schar_type, *uchar_type, *short_type, *ushort_type, *word_type,
		*uword_type, *int_type, *uint_type, *long_type, *ulong_type,
		*lnglng_type, *ulnglng_type,
		*float_type, *double_type, *lngdbl_type, *void_type, *string_type,
		*funint_type, *error_type;

struct type *pa_type; /* Pointer-Arithmetic type	*/

struct type *create_type(int fund)
{
	/*	A brand new struct type is created, and its tp_fund set
	 to fund.
	 */
	register struct type *ntp = new_type();

	ntp->tp_fund = fund;
	ntp->tp_size = (arith) -1;

	return ntp;
}

struct type *promoted_type(struct type *tp)
{
	if (tp->tp_fund == CHAR || tp->tp_fund == SHORT)
	{
		if (tp->tp_unsigned && (int) tp->tp_size == (int) int_size)
			return uint_type;
		else
			return int_type;
	}
	else if (tp->tp_fund == FLOAT)
		return double_type;
	else
		return tp;
}

struct type *construct_type(int fund, register struct type *tp, int qual,
arith count, /* for fund == ARRAY only */
register struct proto *pl)
{
	/*	fund must be a type constructor: FIELD, FUNCTION, POINTER or
	 ARRAY. The pointer to the constructed type is returned.
	 */
	register struct type *dtp;

	switch (fund)
	{
#ifndef NOBITFIELD
	case FIELD:
		dtp = field_of(tp, qual);
		break;
#endif /* NOBITFIELD */

	case FUNCTION:
		if (tp->tp_fund == FUNCTION)
		{
			error("function cannot yield function");
			return error_type;
		}
		if (tp->tp_fund == ARRAY)
		{
			error("function cannot yield array");
			return error_type;
		}

		dtp = function_of(tp, pl, qual);
		break;
	case POINTER:
		dtp = pointer_to(tp, qual);
		break;
	case ARRAY:
		if (tp->tp_fund == VOID)
		{
			error("cannot construct array of void");
			count = (arith) -1;
		}
		dtp = array_of(tp, count, qual);
		break;
	default:
		crash("bad constructor in construct_type");
		/*NOTREACHED*/
	}
	return dtp;
}

struct type *function_of(register struct type *tp, struct proto *pl, int qual)
{
#if 0
	/* See comment below */
	register struct type *dtp = tp->tp_function;
#else
	register struct type *dtp;
#endif

	/* look for a type with the right qualifier */
#if 0
	/* the code doesn't work in the following case:
	 int func();
	 int func(int a, int b) { return q(a); }
	 because updating the type works inside the data-structures for that type
	 thus, a new type is created for very function. This may change in the
	 future, when declarations with empty parameter lists become obsolete.
	 When it does, change type.str, decspecs.c, and this routine. Search for
	 the function_of pattern to find the places.
	 */
	while (dtp && (dtp->tp_typequal != qual || dtp->tp_proto != pl))
	dtp = dtp->next;
#else
	dtp = 0;
#endif

	if (!dtp)
	{
		dtp = create_type(FUNCTION);
		dtp->tp_up = tp;
		dtp->tp_size = -1; /* function size is not known */
		dtp->tp_align = pointer_align;
		dtp->tp_typequal = qual;
		dtp->tp_proto = pl;
#if 0
		/* See comment above */
		dtp->next = tp->tp_function;
		tp->tp_function = dtp;
#endif
	}
	return dtp;
}

struct type *pointer_to(register struct type *tp, int qual)
{
	register struct type *dtp = tp->tp_pointer;

	/* look for a type with the right qualifier */
	while (dtp && dtp->tp_typequal != qual)
		dtp = dtp->next;

	if (!dtp)
	{
		dtp = create_type(POINTER);
		dtp->tp_unsigned = 1;
		dtp->tp_up = tp;
		dtp->tp_size = pointer_size;
		dtp->tp_align = pointer_align;
		dtp->tp_typequal = qual;
		dtp->next = tp->tp_pointer;
		tp->tp_pointer = dtp;
	}
	return dtp;
}

struct type * array_of(register struct type *tp, arith count, int qual)
{
	register struct type *dtp = tp->tp_array;

	/* look for a type with the right size */
	while (dtp && (dtp->tp_nel != count || dtp->tp_typequal != qual))
		dtp = dtp->next;

	if (!dtp)
	{
		dtp = create_type(ARRAY);
		dtp->tp_up = tp;
		dtp->tp_nel = count;
		dtp->tp_align = tp->tp_align;
		dtp->tp_typequal = qual;
		dtp->next = tp->tp_array;
		tp->tp_array = dtp;
		if (tp->tp_size >= 0 && count >= 0)
		{
			dtp->tp_size = count * tp->tp_size;
		}
		else
			dtp->tp_size = -1;
	}
	return dtp;
}

#ifndef NOBITFIELD
struct type * field_of(register struct type *tp, int qual)
{
	register struct type *dtp = create_type(FIELD);

	dtp->tp_up = tp;
	dtp->tp_align = tp->tp_align;
	dtp->tp_size = tp->tp_size;
	dtp->tp_typequal = qual;
	return dtp;
}
#endif /* NOBITFIELD */

arith size_of_type(struct type *tp, char nm[])
{
	arith sz = tp->tp_size;

	if (sz < 0)
	{
		error("size of %s unknown", nm);
		sz = (arith) 1;
	}
	return sz;
}

void idf2type(struct idf *idf, struct type **tpp)
{
	/*	Decoding  a typedef-ed identifier or basic type: if the
	 size is yet unknown we have to make copy of the type
	 descriptor to prevent garbage at the initialisation of
	 arrays with unknown size.
	 */
	register struct type *tp = idf->id_def->df_type;

	if (*tpp)
		error("multiple types in declaration");
	if (tp->tp_size < (arith) 0 && tp->tp_fund == ARRAY)
	{
		*tpp = new_type();
		**tpp = *tp;
		/* this is really a structure assignment, AAGH!!! */
	}
	else
	{
		*tpp = tp;
	}
}

arith align(arith pos, int al)
{
	return ((pos + al - 1) / al) * al;
}

struct type * standard_type(int fund, int sgn, int algn, arith sz)
{
	register struct type *tp = create_type(fund);

	tp->tp_unsigned = sgn != 0;
	tp->tp_align = algn;
	tp->tp_size = sz;

	return tp;
}

void completed(struct type *tp)
{
	register struct type *atp = tp->tp_array;
	register struct type *etp = tp;

	switch (etp->tp_fund)
	{
	case STRUCT:
	case UNION:
	case ENUM:
		while ( (etp = etp->next) !=0)
		{
			if (!etp->tp_sdef)
				etp->tp_sdef = tp->tp_sdef;
			etp->tp_size = tp->tp_size;
			etp->tp_align = tp->tp_align;
		}
		break;
	}
	while (atp)
	{
		if (atp->tp_nel >= 0)
		{
			atp->tp_size = atp->tp_nel * tp->tp_size;
		}
		atp = atp->next;
	}
}

int no_long_long(void)
{
	static int shown = 0;

	if (lnglng_size < 0)
	{
		if (!shown)
		{
			error("no long long for this machine");
			shown = 1;
		}
		return 1;
	}
	else
		return 0;
}
