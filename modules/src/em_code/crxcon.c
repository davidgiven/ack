#include "em_private.h"

/* $Id$ */

void CC_crxcon(int op, int spec, char* v, arith s)
{
	/*	CON or ROM with argument ICON(v,z)
	*/
	PS(op);
	WCON(spec, v, s);
	CEND();
	NL();
}
