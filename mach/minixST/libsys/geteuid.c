#include "lib.h"

PUBLIC uid geteuid()
{
  int k;
  k = callm1(MM, GETUID, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
  if (k < 0) return ( (uid) k);
  return ((uid) M.m2_i1);
}
