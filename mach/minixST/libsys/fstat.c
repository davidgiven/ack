#include "lib.h"

PUBLIC int fstat(fd, buffer)
int fd;
char *buffer;
{
  int n;
  n = callm1(FS, FSTAT, fd, 0, 0, buffer, NIL_PTR, NIL_PTR);
  return(n);
}
