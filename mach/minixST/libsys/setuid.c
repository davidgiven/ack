#include "lib.h"

PUBLIC int setuid(usr)
int usr;
{
  return callm1(MM, SETUID, usr, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
}
