/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
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
