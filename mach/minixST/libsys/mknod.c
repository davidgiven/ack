#include "lib.h"

PUBLIC int mknod(name, mode, addr)
char *name;
int mode, addr;
{
  return callm1(FS, MKNOD, len(name), mode, addr, name, NIL_PTR, NIL_PTR);
}
