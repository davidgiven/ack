#include "lib.h"

PUBLIC int read(fd, buffer, nbytes)
int fd;
char *buffer;
int nbytes;
{
  int n;
  n = callm1(FS, READ, fd, nbytes, 0, buffer, NIL_PTR, NIL_PTR);
  return(n);
}
