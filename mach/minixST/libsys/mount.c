#include "lib.h"

PUBLIC int mount(special, name, rwflag)
char *name, *special;
int rwflag;
{
  return callm1(FS, MOUNT, len(special), len(name), rwflag, special, name, NIL_PTR);
}
