#include <lib.h>
#include <sys/types.h>
#define umask	_umask
#include <sys/stat.h>

PUBLIC mode_t umask(complmode)
int complmode;
{
  return((mode_t)_callm1(FS, UMASK, (int)complmode, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR));
}
