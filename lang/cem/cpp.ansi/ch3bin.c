/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* EVALUATION OF BINARY OPERATORS */

#include	"Lpars.h"
#include	"arith.h"

#define arith_sign (1 << (sizeof(arith)*8-1))

ch3bin(pval, pis_uns, oper, val, is_uns)
	register arith *pval, val;
	int oper, is_uns, *pis_uns;
{
	if (is_uns) *pis_uns = 1;
	switch (oper)	{
	case '/':
		if (val == 0) {
			error("/ by 0");
			break;
		}
		if (*pis_uns) {
#ifdef UNSIGNED_ARITH
			*pval /= (UNSIGNED_ARITH) val;
#else
			/*	this is more of a problem than you might
				think on C compilers which do not have
				unsigned arith (== long (probably)).
			*/
			if (val & arith_sign)	{/* val > max_arith */
				*pval = ! (*pval >= 0 || *pval < val);
				/*	this is the unsigned test
					*pval < val for val > max_arith
				*/
			}
			else	{		/* val <= max_arith */
				arith half, bit, hdiv, hrem, rem;

				half = (*pval >> 1) & ~arith_sign;
				bit = *pval & 01;
				/*	now *pval == 2 * half + bit
					and half <= max_arith
					and bit <= max_arith
				*/
				hdiv = half / val;
				hrem = half % val;
				rem = 2 * hrem + bit;
				*pval = 2 * hdiv + (rem < 0 || rem >= val);
				/*	that is the unsigned compare
					rem >= val for val <= max_arith
				*/
			}
#endif
		}
		else {
			*pval = *pval / val;
		}
		break;
	case '%':
		if (val == 0) {
			error("%% by 0");
			break;
		}
		if (*pis_uns) {
#ifdef UNSIGNED_ARITH
			*pval %= (UNSIGNED_ARITH) val;
#else
			if (val & arith_sign)	{/* val > max_arith */
				*pval = (*pval >= 0 || *pval < val) ? *pval : *pval - val;
				/*	this is the unsigned test
					*pval < val for val > max_arith
				*/
			}
			else	{		/* val <= max_arith */
				arith half, bit, hrem, rem;

				half = (*pval >> 1) & ~arith_sign;
				bit = *pval & 01;
				/*	now *pval == 2 * half + bit
					and half <= max_arith
					and bit <= max_arith
				*/
				hrem = half % val;
				rem = 2 * hrem + bit;
				*pval = (rem < 0 || rem >= val) ? rem - val : rem;
			}
#endif
		}
		else {
			*pval = *pval % val;
		}
		break;
	case '*':
		*pval = *pval * val;
		break;
	case '+':
		*pval = *pval + val;
		break;
	case '-':
		*pval = *pval - val;
		break;
	case LEFT:
		*pval = *pval << val;
		break;
	case RIGHT:
		if (val == 0) break;
		if (*pis_uns) {
			*pval = (*pval >> 1) & ~arith_sign;
			*pval = *pval >> (val - 1);
		}
		else *pval = *pval >> val;
		break;
	case '<':
		{	arith tmp = *pval; *pval = val; val = tmp; }
		/* fall through */
	case '>':
		if (*pis_uns) {
#ifdef UNSIGNED_ARITH
			*pval = (UNSIGNED_ARITH) *pval > (UNSIGNED_ARITH) val;
#else
			*pval = (*pval & arith_sign ?
				(val & arith_sign ? *pval > val : 1) :
				(val & arith_sign ? 0 : *pval > val)
			);
#endif
		}
		else	*pval = (*pval > val);
		break;
	case LESSEQ:
		{	arith tmp = *pval; *pval = val; val = tmp; }
		/* fall through */
	case GREATEREQ:
		if (*pis_uns) {
#ifdef UNSIGNED_ARITH
			*pval = (UNSIGNED_ARITH) *pval >= (UNSIGNED_ARITH) val;
#else
			*pval = (*pval & arith_sign ?
				(val & arith_sign ? *pval >= val : 1) :
				(val & arith_sign ? 0 : *pval >= val)
			);
#endif
		}
		else	*pval = (*pval >= val);
		break;
	case EQUAL:
		*pval = (*pval == val);
		break;
	case NOTEQUAL:
		*pval = (*pval != val);
		break;
	case '&':
		*pval = *pval & val;
		break;
	case '^':
		*pval = *pval ^ val;
		break;
	case '|':
		*pval = *pval | val;
		break;
	case AND:
		*pval = (*pval && val);
		break;
	case OR:
		*pval = (*pval || val);
		break;
	case ',':
		*pis_uns = is_uns;
		*pval = val;
		break;
	}
}
