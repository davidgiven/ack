#include <lib.h>
#define rmdir	_rmdir
#include <unistd.h>

PUBLIC int rmdir(name)
_CONST char *name;
{
  return(_callm3(FS, RMDIR, 0, name));
}
