#include "em_private.h"

/* $Header$ */

void
CC_dlb(l, val)
	label l;
	arith val;
{
	COMMA();
	DOFF(l, val);
}
