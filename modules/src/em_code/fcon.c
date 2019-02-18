#include "em_private.h"

/* $Id$ */

void CC_fcon(char* val, arith siz)
{
	COMMA();
	WCON(sp_fcon, val, siz);
}
