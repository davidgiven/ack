#include "em_private.h"

/* $Id$ */

void CC_ucon(char* val,arith siz)
{
	COMMA();
	WCON(sp_ucon, val, siz);
}
