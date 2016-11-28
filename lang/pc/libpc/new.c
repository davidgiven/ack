#include <stdlib.h>
#include <em_abs.h>
#include <pc_err.h>

extern void _trp(int);			/* called on error */

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