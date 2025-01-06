#include <lib.h>
#define umount	_umount
#include <unistd.h>

PUBLIC int umount(name)
_CONST char *name;
{
  return(_callm3(FS, UMOUNT, 0, name));
}
