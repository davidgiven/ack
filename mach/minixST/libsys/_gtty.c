#include <lib.h>
#define gtty	_gtty
#define ioctl	_ioctl
#include <sgtty.h>

PUBLIC int gtty(fd, argp)
int fd;
struct sgttyb *argp;
{
  return(ioctl(fd, TIOCGETP, argp));
}
