#include "lib.h"

PUBLIC int chmod(name, mode)
char* name;
int mode;
{
  return callm3(FS, CHMOD, mode, name);

}
