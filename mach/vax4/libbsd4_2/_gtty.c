#include <sgtty.h>
int
_gtty(fildes,argp)
	int fildes ;
	struct sgttyb *argp ;
{
	return ioctl(fildes,TIOCGETP,argp) ;
}
