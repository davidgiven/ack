#include "em_private.h"

/* $Header$ */

void
CC_crdnam(op, s, off)
	char *s;
	arith off;
{
	/*	CON or ROM with argument DNAM(s, off)
	*/
	PS(op);
	NOFF(s, off);
	CEND();
	NL();
}
