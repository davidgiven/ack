/* EVALUATION OF MONADIC OPERATORS */

#include	"Lpars.h"

ch7mon(oper, pval)
	register int *pval;
{
	switch (oper)	{
	case '~':
		*pval = ~(*pval);
		break;
	case '-':
		*pval = -(*pval);
		break;
	case '!':
		*pval = !(*pval);
		break;
	}
}
