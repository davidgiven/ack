#include "lib.h"

PUBLIC int exit(status)
int status;
{
  _cleanup();
  return callm1(MM, EXIT, status, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
}
