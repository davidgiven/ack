#include <sgtty.h>
int
gtty(fildes,argp)
	int fildes ;
	struct sgttyb *argp ;
{
	return ioctl(fildes,TIOCGETP,argp) ;
}
