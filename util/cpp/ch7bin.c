/* EVALUATION OF BINARY OPERATORS */

#include	"Lpars.h"

ch7bin(pval, oper, val)
	register int *pval, val;
	int oper;
{
	switch (oper)	{
	case '%':
		if (val == 0)
			error("% by 0");
		else
			*pval = *pval % val;
		break;
	case '/':
		if (val == 0)
			error("/ by 0");
		else
			*pval = *pval / val;
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
		*pval = *pval >> val;
		break;
	case '<':
		*pval = (*pval < val);
		break;
	case '>':
		*pval = (*pval > val);
		break;
	case LESSEQ:
		*pval = (*pval <= val);
		break;
	case GREATEREQ:
		*pval = (*pval >= val);
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
		*pval = val;
		break;
	}
}
