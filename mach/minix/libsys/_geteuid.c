#include <lib.h>
#include <sys/types.h>
#define geteuid	_geteuid

PUBLIC uid_t geteuid()
{
  int k;
  k = _callm1(MM, GETUID, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
  if (k < 0) return((uid_t) k);
  return((uid_t) _M.m2_i1);
}
