#include "em_private.h"

/* $Header$ */

void
CC_crxcon(op, spec, v, s)
	char *v;
	arith s;
{
	/*	CON or ROM with argument ICON(v,z)
	*/
	PS(op);
	WCON(spec, v, s);
	CEND();
	NL();
}
