#include "lib.h"

extern char *brksize;

PUBLIC char *brk(addr)
char *addr;
{
  int k;

  k = callm1(MM, BRK, 0, 0, 0, addr, NIL_PTR, NIL_PTR);
  if (k == OK) {
	brksize = M.m2_p1;
	return(NIL_PTR);
  } else {
	return( (char*) -1 );
  }
}


PUBLIC char *sbrk(incr)
int incr;
{
  char *newsize, *oldsize;
  extern int endv, dorgv;

  oldsize = brksize;
  newsize = brksize + incr;
  if (incr > 0 && newsize < oldsize) return( (char *) -1);
  if (brk(newsize) == 0)
	return(oldsize);
  else
	return( (char *) -1 );
}

