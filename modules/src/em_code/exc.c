#include "em_private.h"

/* $Id$ */

void
CC_exc(c1,c2)
	arith c1,c2;
{
	PS(ps_exc);
	CST(c1);
	COMMA();
	CST(c2);
	NL();
}
