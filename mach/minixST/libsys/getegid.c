#include "lib.h"

PUBLIC gid getegid()
{
  int k;
  k = callm1(MM, GETGID, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
  if (k < 0) return ( (gid) k);
  return( (gid) M.m2_i1);
}
