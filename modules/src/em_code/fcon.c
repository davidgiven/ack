#include "em_private.h"

/* $Id$ */

void
CC_fcon(val, siz)
	char *val;
	arith siz;
{
	COMMA();
	WCON(sp_fcon, val, siz);
}
