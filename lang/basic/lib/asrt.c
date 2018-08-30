#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

void asrt(int b)
{
	if (!b)
	{
		printf("ASSERTION ERROR\n");
		abort();
	}
}
