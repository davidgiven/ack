#include <lib.h>
#define sync	_sync
#include <unistd.h>

PUBLIC int sync()
{
  return(_callm1(FS, SYNC, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR));
}
