#include "em_private.h"

/* $Header$ */

void
CC_end(l)
	arith l;
{
	/*	END pseudo of procedure with l locals
	*/
	PS(ps_end);
	CST(l);
	NL();
}
