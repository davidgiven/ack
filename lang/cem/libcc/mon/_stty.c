/* $Id$ */
#include <sgtty.h>
int
_stty(fildes,argp) int fildes ; struct sgttyb *argp ; {
	return _ioctl(fildes,TIOCSETP,argp) ;
}
