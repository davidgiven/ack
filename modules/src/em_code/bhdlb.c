#include "em_private.h"

/* $Id$ */

void
CC_bhdlb(op, n, s, off, i)
	arith n;
	label s;
	arith off;
	int i;
{
	/*	BSS or HOL with size n, initial value a dlb(s, off),
		and flag i
	*/
	PS(op);
	CST(n);
	COMMA();
	DOFF(s, off);
	COMMA();
	CST((arith) i);
	NL();
}
