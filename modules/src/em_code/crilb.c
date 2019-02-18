#include "em_private.h"

/* $Id$ */

void CC_crilb(int op, label l)
{
	/*	CON or ROM with argument ILB(l)
	*/
	PS(op);
	ILB(l);
	CEND();
	NL();
}
