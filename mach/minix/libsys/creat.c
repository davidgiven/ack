#include "lib.h"

PUBLIC int creat(name, mode)
char* name;
int mode;
{
  return callm3(FS, CREAT, mode, name);
}
