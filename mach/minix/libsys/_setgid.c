#include <lib.h>
#include <sys/types.h>
#define setgid	_setgid

PUBLIC int setgid(grp)
gid_t grp;
{
  return(_callm1(MM, SETGID, (int)grp, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR));
}
