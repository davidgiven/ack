#include "lib.h"
#define brk  _brk
#define sbrk _sbrk
#include <unistd.h>

extern char* _brksize;

PUBLIC int brk(void* addr)
{
	if (_callm1(MM, BRK, 0, 0, 0, addr, NIL_PTR, NIL_PTR) == 0)
	{
		_brksize = _M.m2_p1;
		return 0;
	}
	else
	{
		return -1;
	}
}

PUBLIC void* sbrk(int incr)
{
	char *newsize, *oldsize;

	oldsize = _brksize;
	newsize = _brksize + incr;
	if (incr > 0 && newsize < oldsize || incr < 0 && newsize > oldsize)
		return ((char*)-1);
	if (_brk(newsize) == 0)
		return (oldsize);
	else
		return ((void*)-1);
}
