#include <lib.h>
#define umount	_umount

PUBLIC int umount(name)
char *name;
{
  return(_callm3(FS, UMOUNT, 0, name));
}
