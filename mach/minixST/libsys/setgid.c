#include "lib.h"

PUBLIC int setgid(grp)
int grp;
{
  return callm1(MM, SETGID, grp, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
}
