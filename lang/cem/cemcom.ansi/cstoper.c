/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	C O N S T A N T   E X P R E S S I O N   H A N D L I N G		*/

#include	<assert.h>
#include    "cstoper.h"
#include    "parameters.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"type.h"
#include	"label.h"
#include	"expr.h"
#include	"sizes.h"
#include	"Lpars.h"
#include    "error.h"

/* full_mask[1] == 0XFF, full_mask[2] == 0XFFFF, .. */
arith full_mask[MAXSIZE + 1];
#ifndef NOCROSS
arith max_int;		/* maximum integer on target machine	*/
arith max_unsigned;	/* maximum unsigned on target machine	*/
#endif /* NOCROSS */
extern int ResultKnown;

void cstbin(register struct expr **expp, int oper, register struct expr *expr)
{
	/*	The operation oper is performed on the constant
		expressions *expp(ld) and expr(ct), and the result restored in
		*expp.
	*/
	register arith o1 = (*expp)->VL_VALUE;
	register arith o2 = expr->VL_VALUE;
	int uns = (*expp)->ex_type->tp_unsigned;

	assert(is_ld_cst(*expp) && is_cp_cst(expr));
	switch (oper)	{
	case '*':
		o1 *= o2;
		break;
	case '/':
		if (o2 == 0)	{
			if (!ResultKnown)
				expr_error(expr, "division by 0");
			else
				expr_warning(expr, "division by 0");
			break;
		}
		if (uns)
			o1 /= (unsigned arith) o2;
		else
			o1 /= o2;
		break;
	case '%':
		if (o2 == 0)	{
			if (!ResultKnown)
				expr_error(expr, "modulo by 0");
			else
				expr_warning(expr, "modulo by 0");
			break;
		}
		if (uns)
			o1 %= (unsigned arith) o2;
		else
			o1 %= o2;
		break;
	case '+':
		o1 += o2;
		break;
	case '-':
		o1 -= o2;
		break;
	case LEFT:
		o1 <<= o2;
		break;
	case RIGHT:
		if (o2 == 0)
			break;
		if (uns)	{
			o1 = (o1 >> 1) & ~arith_sign;
			o1 >>= (o2 - 1);
		}
		else	o1 >>= o2;
		break;
	case '<':
		{
			arith tmp = o1;

			o1 = o2;
			o2 = tmp;
		}
		/* Fall through */
	case '>':
		if (uns)
			o1 = (unsigned arith) o1 > (unsigned arith) o2;
		else
			o1 = o1 > o2;
		break;
	case LESSEQ:
		{
			arith tmp = o1;

			o1 = o2;
			o2 = tmp;
		}
		/* Fall through */
	case GREATEREQ:
		if (uns)
			o1 = (unsigned arith) o1 >= (unsigned arith) o2;
		else
			o1 = o1 >= o2;
		break;
	case EQUAL:
		o1 = o1 == o2;
		break;
	case NOTEQUAL:
		o1 = o1 != o2;
		break;
	case '&':
		o1 &= o2;
		break;
	case '|':
		o1 |= o2;
		break;
	case '^':
		o1 ^= o2;
		break;
	}
	(*expp)->VL_VALUE = o1;
	cut_size(*expp);
	(*expp)->ex_flags |= expr->ex_flags;
	(*expp)->ex_flags &= ~EX_PARENS;
	free_expression(expr);
}

void cut_size(register struct expr *expr)
{
	/*	The constant value of the expression expr is made to
		conform to the size of the type of the expression.
	*/
	register arith o1 = expr->VL_VALUE;
	int uns = expr->ex_type->tp_unsigned;
	int size = (int) expr->ex_type->tp_size;

	assert(expr->ex_class == Value);
	if (expr->ex_type->tp_fund == POINTER) {
		/* why warn on "ptr-3" ?
		   This quick hack fixes it
		*/
		uns = 0;
	}
	if (uns) {
		if (o1 & ~full_mask[size])
		    if (!ResultKnown)
			expr_warning(expr,
				"overflow in unsigned constant expression");
		o1 &= full_mask[size];
	}
	else {
		int nbits = (int) (arith_size - size) * 8;
		arith remainder = o1 & ~full_mask[size];

		if (remainder != 0 && remainder != ~full_mask[size])
		    if (!ResultKnown)
			expr_warning(expr,"overflow in constant expression");
		o1 = (o1 << nbits) >> nbits;		/* ??? */
	}
	expr->VL_VALUE = o1;
}

void init_cst(void)
{
	register int i = 0;
	register arith bt = (arith)0;

	/*	FIXME arith is insufficient for long long.  We ignore
		this problem and write masks up to full_mask[8], but
		masks are wrong after bt < 0.
	*/
	while (!(bt < 0) || i < 8) {
		bt = (bt << 8) + 0377, i++;
		if (i > MAXSIZE)
			fatal("array full_mask too small for this machine");
		full_mask[i] = bt;
	}
	if ((int)long_size > arith_size)
		fatal("sizeof (arith) insufficient on this machine");
#ifndef NOCROSS
	max_int = full_mask[(int)int_size] & ~(1L << ((int)int_size * 8 - 1));
	max_unsigned = full_mask[(int)int_size];
#endif /* NOCROSS */
}
