#include "em_private.h"

/* $Id$ */

void
CC_crdlb(op, v, off)
	label v;
	arith off;
{
	/*	CON or ROM with argument DLB(v, off)
	*/
	PS(op);
	DOFF(v, off);
	CEND();
	NL();
}
