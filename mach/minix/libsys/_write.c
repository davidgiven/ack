#include <lib.h>
#define write	_write

PUBLIC int write(fd, buffer, nbytes)
int fd;
char *buffer;
unsigned nbytes;
{
  return(_callm1(FS, WRITE, fd, nbytes, 0, buffer, NIL_PTR, NIL_PTR));
}
