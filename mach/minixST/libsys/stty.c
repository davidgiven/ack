#include <sgtty.h>

stty(fd, argp)
int fd;
char *argp;
{
  return ioctl(fd, TIOCSETP, argp);
}

