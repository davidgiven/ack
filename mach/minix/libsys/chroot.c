#include "lib.h"

PUBLIC int chroot(name)
char* name;
{
  return callm3(FS, CHROOT, 0, name);

}
