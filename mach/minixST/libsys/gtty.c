#include <sgtty.h>

gtty(fd, argp)
int fd;
char *argp;
{
  return ioctl(fd, TIOCGETP, argp);
}

