#include <lib.h>
#define getppid	_getppid
#include <unistd.h>

PUBLIC int getppid()
{
  int p;

  p = _callm1(MM, GETPID, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
  if (p < 0) return(p);
  return(_M.m2_i1);
}
