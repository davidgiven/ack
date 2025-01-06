#include <lib.h>
#define stty	_stty
#define ioctl	_ioctl
#include <sgtty.h>

PUBLIC int stty(fd, argp)
int fd;
struct sgttyb *argp;
{
  return ioctl(fd, TIOCSETP, argp);
}
