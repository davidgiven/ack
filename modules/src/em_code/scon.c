#include "em_private.h"

/* $Id$ */

void CC_scon(char* str, arith siz)
{
	COMMA();
	SCON(str, siz);
}
