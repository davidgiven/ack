#include <lib.h>
#define brk	_brk
#define sbrk	_sbrk

extern char *_brksize;

PUBLIC char *brk(addr)
char *addr;
{
  if (_callm1(MM, BRK, 0, 0, 0, addr, NIL_PTR, NIL_PTR) == 0) {
	_brksize = _M.m2_p1;
	return(NIL_PTR);
  } else {
	return((char *) -1);
  }
}


PUBLIC char *sbrk(incr)
int incr;
{
  char *newsize, *oldsize;

  oldsize = _brksize;
  newsize = _brksize + incr;
  if (incr > 0 && newsize < oldsize || incr < 0 && newsize > oldsize)
	return((char *) -1);
  if (_brk(newsize) == 0)
	return(oldsize);
  else
	return((char *) -1);
}
