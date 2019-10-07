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
writh full_mask[MAXSIZE + 1];
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
	unsigned writh o1 = (unsigned writh)(*expp)->VL_VALUE;
	unsigned writh o2 = (unsigned writh)expr->VL_VALUE;
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
			o1 /= o2;
		else
			o1 = (unsigned writh)((writh)o1 / (writh)o2);
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
			o1 %= o2;
		else
			o1 = (unsigned writh)((writh)o1 % (writh)o2);
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
		if (uns)
			o1 >>= o2;
		else
			o1 = (unsigned writh)((writh)o1 >> (writh)o2);
		break;
	case '<':
		{
			writh tmp = o1;

			o1 = o2;
			o2 = tmp;
		}
		/* Fall through */
	case '>':
		if (uns)
			o1 = o1 > o2;
		else
			o1 = (writh)o1 > (writh)o2;
		break;
	case LESSEQ:
		{
			writh tmp = o1;

			o1 = o2;
			o2 = tmp;
		}
		/* Fall through */
	case GREATEREQ:
		if (uns)
			o1 = o1 >= o2;
		else
			o1 = (writh)o1 >= (writh)o2;
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
	(*expp)->VL_VALUE = (writh)o1;
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
	writh o1 = expr->VL_VALUE;
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
		int nbits = (int) (sizeof(o1) - size) * 8;
		writh remainder = o1 & ~full_mask[size];

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
	unsigned writh bt = 0;

	while (!((writh)bt < 0)) {
		bt = (bt << 8) + 0377, i++;
		if (i > MAXSIZE)
			fatal("array full_mask too small for this machine");
		full_mask[i] = bt;
	}
	/* signed comparison; lnglng_size might be -1 */
	if (long_size > (arith)sizeof(writh) ||
	    lnglng_size > (arith)sizeof(writh))
		fatal("sizeof(writh) insufficient on this machine");
#ifndef NOCROSS
	max_int = (arith)((unsigned writh)full_mask[(int)int_size] >> 1);
	max_unsigned = (arith)full_mask[(int)int_size];
#endif /* NOCROSS */
}
