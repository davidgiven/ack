#include "em_private.h"

/* $Id$ */

void CC_dlb(label l, arith val)
{
	COMMA();
	DOFF(l, val);
}
