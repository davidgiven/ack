#include <lib.h>
#define stime _stime

PUBLIC int stime(top)
long *top;
{
  _M.m2_l1 = *top;
  return(_callx(FS, STIME));
}
