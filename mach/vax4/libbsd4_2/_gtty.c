#include <sgtty.h>
int
_gtty(fildes,argp)
	int fildes ;
	struct sgttyb *argp ;
{
	return _ioctl(fildes,TIOCGETP,argp) ;
}
