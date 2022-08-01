/* $Id$ */
#include <sgtty.h>

int stty(int fildes, struct sgttyb* argp) {
	return ioctl(fildes,TIOCSETP,argp) ;
}
