/* $Header$ */

#include <system.h>

int
sys_read(fp, bufptr, bufsiz, pnbytes)
	File *fp;
	char *bufptr;
	int bufsiz, *pnbytes;
{
	return (*pnbytes = read(fp->o_fd, bufptr, bufsiz)) >= 0;
}
