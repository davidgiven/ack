#include <lib.h>
#define fork	_fork

PUBLIC int fork()
{
  return(_callm1(MM, FORK, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR));
}
