#include <lib.h>
#define pause	_pause

PUBLIC int pause()
{
  return(_callm1(MM, PAUSE, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR));
}
