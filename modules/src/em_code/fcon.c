#include "em_private.h"

/* $Header$ */

CC_fcon(val, siz)
	char *val;
	arith siz;
{
	COMMA();
	WCON(sp_fcon, val, siz);
}
