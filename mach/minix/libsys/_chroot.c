#include <lib.h>
#define chroot	_chroot

PUBLIC int chroot(name)
char *name;
{
  return(_callm3(FS, CHROOT, 0, name));
}
