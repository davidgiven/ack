#include "lib.h"

PUBLIC int open(name, mode)
char* name;
int mode;
{
  return callm3(FS, OPEN, mode, name);
}
