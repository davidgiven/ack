#include <sgtty.h>
int
_gtty(fildes,argp)
	int fildes ;
	struct sgttyb *argp ;
{
	return _ioctl(fildes,0x40067408,argp) ;
}
