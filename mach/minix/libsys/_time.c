#include <lib.h>
#define time	_time
#include <time.h>

PUBLIC long time(tp)
long *tp;
{
  int k;
  long l;
  k = _callm1(FS, TIME, 0, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);
  if (_M.m_type < 0 || k != 0) {
	errno = -_M.m_type;
	return(-1L);
  }
  l = _M.m2_l1;
  if (tp != (long *) 0) *tp = l;
  return(l);
}
