#include <lib.h>
#define mkdir	_mkdir
#include <sys/stat.h>

PUBLIC int mkdir(name, mode)
_CONST char *name;
int mode;
{
  return(_callm1(FS, MKDIR, _len(name), mode, 0, (char *)name, NIL_PTR, NIL_PTR));
}
