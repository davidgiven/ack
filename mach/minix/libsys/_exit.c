#include "lib.h"

PUBLIC int _exit(status)
int status;
{
  return callm1(MM, EXIT, status, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
}
