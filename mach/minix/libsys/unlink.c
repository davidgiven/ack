#include "lib.h"

PUBLIC int unlink(name)
char *name;
{
  return callm3(FS, UNLINK, 0, name);
}
