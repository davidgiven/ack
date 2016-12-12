#include <stdlib.h>

void _hlt(int nr)
{
	exit(nr);
}

void _goto_err(void)
{
	error(3);
}
