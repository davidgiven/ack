#include "em_private.h"

/* $Id$ */

void
CC_scon(str, siz)
	char *str;
	arith siz;
{
	COMMA();
	SCON(str, siz);
}
