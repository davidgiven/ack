#include "em_private.h"

/* $Id$ */

void CC_bhpnam(int op, arith n, char *p, int i)
{
	/*	BSS or HOL with size n, initial value a PNAM(p),
		and flag i
	*/
	PS(op);
	CST(n);
	COMMA();
	PNAM(p);
	COMMA();
	CST((arith) i);
	NL();
}
