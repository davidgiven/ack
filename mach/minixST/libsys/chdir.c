#include "lib.h"

PUBLIC int chdir(name)
char *name;
{
  return callm3(FS, CHDIR, 0, name);

}
