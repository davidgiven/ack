#include <lib.h>
#include <sys/types.h>
#define getuid	_getuid

PUBLIC uid_t getuid()
{
  return((uid_t)_callm1(MM, GETUID, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR));
}
