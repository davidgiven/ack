#include "em_private.h"

/* $Header$ */

void
CC_crcst(op, v)
	arith v;
{
	/*	CON or ROM with argument CST(v)
	*/
	PS(op);
	CST(v);
	CEND();
	NL();
}
