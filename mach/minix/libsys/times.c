#include "lib.h"
#include <sys/types.h>
#include <sys/times.h>

PUBLIC int times(buf)
struct tms *buf;
{
  int k;
  k = callm1(FS, TIMES, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
  buf->tms_utime = M.m4_l1;
  buf->tms_stime = M.m4_l2;
  buf->tms_cutime = M.m4_l3;
  buf->tms_cstime = M.m4_l4;
  return(k);
}
