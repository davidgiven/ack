#include "lib.h"

PUBLIC int stat(name, buffer)
char *name;
char *buffer;
{
  int n;
  n = callm1(FS, STAT, len(name), 0, 0, name, buffer, NIL_PTR);
  return(n);
}
