#include "em_private.h"

/* $Id$ */

void CC_end(arith l)
{
	/*	END pseudo of procedure with l locals
	*/
	PS(ps_end);
	CST(l);
	NL();
}
