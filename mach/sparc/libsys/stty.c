#include <sgtty.h>
int stty(fildes,argp)
	int fildes ;
	struct sgttyb *argp ;
{
	return ioctl(fildes,TIOCSETP,argp) ;
}
