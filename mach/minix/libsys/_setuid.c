#include <lib.h>
#include <sys/types.h>
#define setuid	_setuid

PUBLIC int setuid(usr)
int usr;
{
  return(_callm1(MM, SETUID, (int)usr, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR));
}
