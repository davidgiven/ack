#include "em_private.h"

/* $Header$ */

void
CC_ucon(val,siz)
	char *val;
	arith siz;
{
	COMMA();
	WCON(sp_ucon, val, siz);
}
