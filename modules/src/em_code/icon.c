#include "em_private.h"

/* $Header$ */

void
CC_icon(val, siz)
	char *val;
	arith siz;
{
	COMMA();
	WCON(sp_icon, val, siz);
}
