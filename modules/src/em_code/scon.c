#include "em_private.h"

/* $Header$ */

void
CC_scon(str, siz)
	char *str;
	arith siz;
{
	COMMA();
	SCON(str, siz);
}
