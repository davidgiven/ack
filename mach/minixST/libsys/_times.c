#include <lib.h>
#include <sys/types.h>
#include <time.h>
#define times	_times
#include <sys/times.h>

PUBLIC clock_t times(buf)
struct tms *buf;
{
  clock_t k;
  k = (clock_t)_callm1(FS, TIMES, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
  buf->tms_utime = _M.m4_l1;
  buf->tms_stime = _M.m4_l2;
  buf->tms_cutime = _M.m4_l3;
  buf->tms_cstime = _M.m4_l4;
  return(k);
}
