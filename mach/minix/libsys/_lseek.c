#include <lib.h>
#include <sys/types.h>
#define lseek	_lseek

PUBLIC off_t lseek(fd, offset, whence)
int fd;
off_t offset;
int whence;
{
  int k;
  _M.m2_i1 = fd;
  _M.m2_l1 = offset;
  _M.m2_i2 = whence;
  k = _callx(FS, LSEEK);
  if (k != 0) return((off_t) k);	/* _send() itself failed */
  return((off_t)_M.m2_l1);
}
