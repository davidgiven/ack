/* $Id$ */
#include <sgtty.h>

int gtty(int fildes, struct sgttyb* argp) {
	return ioctl(fildes,TIOCGETP,argp) ;
}
