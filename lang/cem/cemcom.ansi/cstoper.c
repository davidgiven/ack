/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	C O N S T A N T   E X P R E S S I O N   H A N D L I N G		*/

#include	"trgt_sizes.h"
#include	"idf.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"type.h"
#include	"label.h"
#include	"expr.h"
#include	"sizes.h"
#include	"Lpars.h"
#include	"assert.h"

arith full_mask[MAXSIZE];/* full_mask[1] == 0XFF, full_mask[2] == 0XFFFF, .. */
#ifndef NOCROSS
arith max_int;		/* maximum integer on target machine	*/
arith max_unsigned;	/* maximum unsigned on target machine	*/
#endif /* NOCROSS */
extern int ResultKnown;

cstbin(expp, oper, expr)
	register struct expr **expp, *expr;
{
	/*	The operation oper is performed on the constant
		expressions *expp(ld) and expr(ct), and the result restored in
		*expp.
	*/
	register arith o1 = (*expp)->VL_VALUE;
	register arith o2 = expr->VL_VALUE;
	int uns = (*expp)->ex_type->tp_unsigned;

	ASSERT(is_ld_cst(*expp) && is_cp_cst(expr));
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
		if (uns)	{
#ifdef UNSIGNED_ARITH
			o1 /= (UNSIGNED_ARITH) o2;
#else
			/*	this is more of a problem than you might
				think on C compilers which do not have
				unsigned arith (== long (probably)).
			*/
			if (o2 & arith_sign)	{/* o2 > max_arith */
				o1 = ! (o1 >= 0 || o1 < o2);
				/*	this is the unsigned test
					o1 < o2 for o2 > max_arith
				*/
			}
			else	{		/* o2 <= max_arith */
				arith half, bit, hdiv, hrem, rem;

				half = (o1 >> 1) & ~arith_sign;
				bit = o1 & 01;
				/*	now o1 == 2 * half + bit
					and half <= max_arith
					and bit <= max_arith
				*/
				hdiv = half / o2;
				hrem = half % o2;
				rem = 2 * hrem + bit;
				o1 = 2 * hdiv + (rem < 0 || rem >= o2);
				/*	that is the unsigned compare
					rem >= o2 for o2 <= max_arith
				*/
			}
#endif
		}
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
		if (uns)	{
#ifdef UNSIGNED_ARITH
			o1 %= (UNSIGNED_ARITH) o2;
#else
			if (o2 & arith_sign)	{/* o2 > max_arith */
				o1 = (o1 >= 0 || o1 < o2) ? o1 : o1 - o2;
				/*	this is the unsigned test
					o1 < o2 for o2 > max_arith
				*/
			}
			else	{		/* o2 <= max_arith */
				arith half, bit, hrem, rem;

				half = (o1 >> 1) & ~arith_sign;
				bit = o1 & 01;
				/*	now o1 == 2 * half + bit
					and half <= max_arith
					and bit <= max_arith
				*/
				hrem = half % o2;
				rem = 2 * hrem + bit;
				o1 = (rem < 0 || rem >= o2) ? rem - o2 : rem;
			}
#endif
		}
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
		if (uns)	{
#ifdef UNSIGNED_ARITH
			o1 = (UNSIGNED_ARITH) o1 > (UNSIGNED_ARITH) o2;
#else
			o1 = (o1 & arith_sign ?
				(o2 & arith_sign ? o1 > o2 : 1) :
				(o2 & arith_sign ? 0 : o1 > o2)
			);
#endif
		}
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
		if (uns)	{
#ifdef UNSIGNED_ARITH
			o1 = (UNSIGNED_ARITH) o1 >= (UNSIGNED_ARITH) o2;
#else
			o1 = (o1 & arith_sign ?
				(o2 & arith_sign ? o1 >= o2 : 1) :
				(o2 & arith_sign ? 0 : o1 >= o2)
			);
#endif
		}
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

cut_size(expr)
	register struct expr *expr;
{
	/*	The constant value of the expression expr is made to
		conform to the size of the type of the expression.
	*/
	register arith o1 = expr->VL_VALUE;
	int uns = expr->ex_type->tp_unsigned;
	int size = (int) expr->ex_type->tp_size;

	ASSERT(expr->ex_class == Value);
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

init_cst()
{
	register int i = 0;
	register arith bt = (arith)0;

	while (!(bt < 0))	{
		bt = (bt << 8) + 0377, i++;
		if (i == MAXSIZE)
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
