#include <sgtty.h>
int
gtty(fildes,argp)
	int fildes ;
	struct sgttyb *argp ;
{
	return ioctl(fildes,0x40067408,argp) ;
}
