#include "lib.h"

PUBLIC int fork()
{
  return callm1(MM, FORK, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
}
