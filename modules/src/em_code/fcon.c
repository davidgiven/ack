#include "em_private.h"

/* $Header$ */

void
CC_fcon(val, siz)
	char *val;
	arith siz;
{
	COMMA();
	WCON(sp_fcon, val, siz);
}
