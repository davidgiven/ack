#include "em_private.h"

/* $Id$ */

void CC_icon(char* val, arith siz)
{
	COMMA();
	WCON(sp_icon, val, siz);
}
