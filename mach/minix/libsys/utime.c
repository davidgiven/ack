#include "lib.h"

PUBLIC int utime(name, timp)
char *name;
long timp[2];
{
  M.m2_i1 = len(name);
  M.m2_l1 = timp[0];
  M.m2_l2 = timp[1];
  M.m2_p1 = name;
  return callx(FS, UTIME);
}
