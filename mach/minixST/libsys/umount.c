#include "lib.h"

PUBLIC int umount(name)
char* name;
{
  return callm3(FS, UMOUNT, 0, name);
}
