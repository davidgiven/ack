#include <lib.h>
#define ptrace _ptrace

PUBLIC long ptrace(req, pid, addr, data)
int req, pid;
long addr, data;
{
  _M.m2_i1 = pid;
  _M.m2_i2 = req;
  _M.m2_l1 = addr;
  _M.m2_l2 = data;
  if (_callx(MM, PTRACE) == -1) return(-1L);
  if (_M.m2_l2 == -1) {
	errno = 0;
	return(-1L);
  }
  return(_M.m2_l2);
}
