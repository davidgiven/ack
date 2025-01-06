#include <lib.h>
#define getpid	_getpid
#include <unistd.h>

PUBLIC int getpid()
{
  return(_callm1(MM, GETPID, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR));
}
