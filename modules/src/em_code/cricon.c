#include "em_private.h"

CC_cricon(op, v, s)
	char *v;
	arith s;
{
	/*	CON or ROM with argument ICON(v,z)
	*/
	PS(op);
	WCON(sp_icon, v, s);
	CEND();
	NL();
}
