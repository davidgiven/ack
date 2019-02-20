/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	D E C L A R A T I O N   S P E C I F I E R   C H E C K I N G	*/

#include	<assert.h>
#include	"decspecs.h"
#include	"Lpars.h"
#include	"arith.h"
#include	"type.h"
#include	"level.h"
#include	"def.h"
#include    "error.h"

extern char options[];
extern int level;
extern char *symbol2str();
extern struct type *qualifier_type();

struct decspecs null_decspecs;

void do_decspecs(register struct decspecs *ds)
{
	/*	The provisional decspecs ds as obtained from the program
	 is turned into a legal consistent decspecs.
	 */
	register struct type *tp = ds->ds_type;

	assert(level != L_FORMAL1);

	if (level == L_GLOBAL && (ds->ds_sc == AUTO || ds->ds_sc == REGISTER))
	{
		error("no global %s variable allowed", symbol2str(ds->ds_sc));
		ds->ds_sc = GLOBAL;
	}

	if (level == L_FORMAL2)
	{
		if (ds->ds_sc_given && ds->ds_sc != REGISTER)
		{
			error("%s formal illegal", symbol2str(ds->ds_sc));
			ds->ds_sc = FORMAL;
		}
	}

	/*	Since type qualifiers may be associated with types by means
	 of typedefs, we have to perform same basic tests down here.
	 */
	if (tp != (struct type *) 0)
	{
		if ((ds->ds_typequal & TQ_VOLATILE) && (tp->tp_typequal & TQ_VOLATILE))
			error("indirect repeated type qualifier");
		if ((ds->ds_typequal & TQ_CONST) && (tp->tp_typequal & TQ_CONST))
			error("indirect repeated type qualifier");
		ds->ds_typequal |= tp->tp_typequal;
	}

	/*	The tests concerning types require a full knowledge of the
	 type and will have to be postponed to declare_idf.
	 */

	/* some adjustments as described in 3.5.2. */
	if (tp == 0)
	{
		ds->ds_notypegiven = 1;
		tp = int_type;
	}
	if (ds->ds_size)
	{
		register int ds_isshort = (ds->ds_size == SHORT);

		if (ds->ds_typedef)
			goto SIZE_ERROR;
		/* yes */
		if (tp == int_type)
		{
			if (ds_isshort)
				tp = short_type;
			else
				tp = long_type;
		}
		else if (tp == double_type && !ds_isshort)
		{
			tp = lngdbl_type;
		}
		else
		{
			SIZE_ERROR: error("%s with illegal type", symbol2str(ds->ds_size));
		}
		ds->ds_notypegiven = 0;
	}
	if (ds->ds_unsigned)
	{
		register int ds_isunsigned = (ds->ds_unsigned == UNSIGNED);

		if (ds->ds_typedef)
			goto SIGN_ERROR;
		/* yes */
		/*
		 * All integral types are signed by default (char too),
		 * so the case that ds->ds_unsigned == SIGNED can be ignored.
		 */
		if (tp == schar_type)
		{
			if (ds_isunsigned)
				tp = uchar_type;
		}
		else if (tp == short_type)
		{
			if (ds_isunsigned)
				tp = ushort_type;
		}
		else if (tp == int_type)
		{
			if (ds_isunsigned)
				tp = uint_type;
		}
		else if (tp == long_type)
		{
			if (ds_isunsigned)
				tp = ulong_type;
		}
		else
		{
			SIGN_ERROR: error("%s with illegal type",
					symbol2str(ds->ds_unsigned));
		}
		ds->ds_notypegiven = 0;
	}
	ds->ds_type = qualifier_type(tp, ds->ds_typequal);
}

/*	Make tp into a qualified type. This is not as trivial as it
 may seem. If tp is a fundamental type the qualified type is
 either existent or will be generated.
 In case of a complex type the top of the type list will be
 replaced by a qualified version.
 */
struct type *qualifier_type(register struct type *tp, int typequal)
{
	register struct type *dtp = tp;
	register int fund = tp->tp_fund;

	while (dtp && dtp->tp_typequal != typequal)
		dtp = dtp->next;

	if (!dtp)
	{
		dtp = create_type(fund);
		dtp->tp_unsigned = tp->tp_unsigned;
		dtp->tp_align = tp->tp_align;
		dtp->tp_typequal = typequal;
		dtp->tp_size = tp->tp_size;
#if 0
		/* The tp_function field does not exist now. See the comment in the
		 function_of() routine.
		 */
		dtp->tp_function = tp->tp_function;
#endif
		switch (fund)
		{
		case ARRAY:
			if (typequal)
			{
				tp->tp_up = qualifier_type(tp->tp_up, typequal);
				dtp->tp_typequal = typequal = 0;
			}
			goto nottagged;
		case FIELD:
			dtp->tp_field = tp->tp_field;
			/* fallthrough */
		case POINTER:
		case FUNCTION: /* dont't assign tp_proto */
			nottagged: dtp->tp_up = tp->tp_up;
			break;
		case STRUCT:
		case UNION:
		case ENUM:
			dtp->tp_idf = tp->tp_idf;
			dtp->tp_sdef = tp->tp_sdef;
			break;
		default:
			break;
		}
		dtp->next = tp->next; /* don't know head or tail */
		tp->next = dtp;
	}
	return (dtp);
}

