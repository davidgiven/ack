#include <lib.h>
#define close	_close
#include <unistd.h>

PUBLIC int close(fd)
int fd;
{
  return(_callm1(FS, CLOSE, fd, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR));
}
