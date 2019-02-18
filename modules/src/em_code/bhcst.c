#include "em_private.h"

/* $Id$ */

void CC_bhcst(int op, arith n, arith w, int i)
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
