#include "em_private.h"

CC_crucon(op, v, s)
	char *v;
	arith s;
{
	/*	CON or ROM with argument UCON(v,z)
	*/
	PS(op);
	WCON(sp_ucon, v, s);
	CEND();
	NL();
}
