#include <sgtty.h>
int stty(fildes,argp)
	int fildes ;
	struct sgttyb *argp ;
{
	return ioctl(fildes,0x80067409,argp) ;
}
