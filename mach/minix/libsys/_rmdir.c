#include <lib.h>
#define rmdir	_rmdir

PUBLIC int rmdir(name)
char *name;
{
  return(_callm3(FS, RMDIR, 0, name));
}
