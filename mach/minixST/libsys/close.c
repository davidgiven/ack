#include "lib.h"

PUBLIC int close(fd)
int fd;
{
  return callm1(FS, CLOSE, fd, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR);

}
