#include "lib.h"

PUBLIC int access(name, mode)
char *name;
int mode;
{
  return callm3(FS, ACCESS, mode, name);

}
