#include "lib.h"

PUBLIC uid getuid()
{
  int k;
  k = callm1(MM, GETUID, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
  return( (uid) k);
}
