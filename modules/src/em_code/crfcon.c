#include "em_private.h"

CC_crfcon(op, v, s)
	char *v;
	arith s;
{
	/*	CON or ROM with argument FCON(v,z)
	*/
	PS(op);
	WCON(sp_fcon, v, s);
	CEND();
	NL();
}
