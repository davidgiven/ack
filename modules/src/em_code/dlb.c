#include "em_private.h"

/* $Header$ */

CC_dlb(l, val)
	label l;
	arith val;
{
	COMMA();
	DOFF(l, val);
}
