#include "lib.h"

PUBLIC int stime(top)
long *top;
{
  M.m2_l1 = *top;
  return callx(FS, STIME);
}
