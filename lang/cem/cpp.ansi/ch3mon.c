/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* EVALUATION OF MONADIC OPERATORS */

#include	"Lpars.h"
#include	"arith.h"

/*ARGSUSED2*/
ch3mon(oper, pval, puns)
	register arith *pval;
	int *puns;
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
