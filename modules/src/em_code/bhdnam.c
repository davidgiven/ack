#include "em_private.h"

/* $Header$ */

void
CC_bhdnam(op, n, s, off, i)
	arith n;
	char *s;
	arith off;
	int i;
{
	/*	BSS or HOL with size n, initial value a dnam(s, off),
		and flag i
	*/
	PS(op);
	CST(n);
	COMMA();
	NOFF(s, off);
	COMMA();
	CST((arith) i);
	NL();
}
