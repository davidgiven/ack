/* $Id$ */
/*
 * return offset in file.
 */

long	lseek(int fd, int offset, int whence);

long tell(int fd)
{
	return(lseek(fd, 0L, 1));
}
