#include "lib.h"

PUBLIC int (*__cleanup)();

PUBLIC int exit(status)
int status;
{
  if (__cleanup) (*__cleanup)();
  return callm1(MM, EXIT, status, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
}
