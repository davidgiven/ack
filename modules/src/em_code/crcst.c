#include "em_private.h"

/* $Id$ */

void CC_crcst(int op, arith v)
{
	/*	CON or ROM with argument CST(v)
	*/
	PS(op);
	CST(v);
	CEND();
	NL();
}
