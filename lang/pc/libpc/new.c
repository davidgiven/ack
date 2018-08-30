#include <stdlib.h>
#include <em_abs.h>
#include "pc.h"

void _new(int n, void** ptr)
{
	void* p = malloc(n);
	if (!p)
		_trp(EHEAP);

	*ptr = p;
}

void _dis(int n, void** ptr)
{
	free(*ptr);
	*ptr = NULL;
}