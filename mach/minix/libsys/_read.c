#include <lib.h>
#define read	_read

PUBLIC int read(fd, buffer, nbytes)
int fd;
char *buffer;
unsigned nbytes;
{
  return(_callm1(FS, READ, fd, nbytes, 0, buffer, NIL_PTR, NIL_PTR));
}
