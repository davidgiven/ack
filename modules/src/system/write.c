/* $Header$ */

#include <system.h>

int
sys_write(fp, bufptr, nbytes)
	File *fp;
	char *bufptr;
	int nbytes;
{
	return write(fp->o_fd, bufptr, nbytes) == nbytes;
}
