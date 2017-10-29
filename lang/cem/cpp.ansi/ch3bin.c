/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* EVALUATION OF BINARY OPERATORS */

#include	"Lpars.h"
#include	"arith.h"

#define arith_sign (1L << (sizeof(arith)*8-1))

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
			*pval /= (unsigned arith) val;
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
			*pval %= (unsigned arith) val;
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
			*pval = (unsigned arith) *pval > (unsigned arith) val;
		}
		else	*pval = (*pval > val);
		break;
	case LESSEQ:
		{	arith tmp = *pval; *pval = val; val = tmp; }
		/* fall through */
	case GREATEREQ:
		if (*pis_uns) {
			*pval = (unsigned arith) *pval >= (unsigned arith) val;
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
