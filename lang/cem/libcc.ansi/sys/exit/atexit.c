/* $Id$ */

#include <stdlib.h>
#include "atexits.h"

int atexit(void (*func)(void))
{
	if (__funccnt >= NEXITS)
		return 1;
	__functab[__funccnt++] = func;
	return 0;
}
