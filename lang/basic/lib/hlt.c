#include <stdlib.h>
#include "lib.h"

void _hlt(int nr)
{
	exit(nr);
}

void _goto_err(void)
{
	error(3);
}
