#include "em_private.h"

/* $Header$ */

void
CC_bhcst(op, n, w, i)
	arith n;
	arith w;
	int i;
{
	/*	BSS or HOL with size n, initial value a cst w, and flag i
	*/
	PS(op);
	CST(n);
	COMMA();
	CST(w);
	COMMA();
	CST((arith) i);
	NL();
}
