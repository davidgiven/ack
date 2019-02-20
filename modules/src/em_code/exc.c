#include "em_private.h"

/* $Id$ */

void CC_exc(arith c1,arith c2)
{
	PS(ps_exc);
	CST(c1);
	COMMA();
	CST(c2);
	NL();
}
