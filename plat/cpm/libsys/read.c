/* $Source$
 * $State$
 * $Revision$
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <cpm.h>

ssize_t read(int fd, void* buffer, size_t count)
{
	short save;
	unsigned char before_n;

	if (fd != 0)
	{
		errno = EBADF;
		return -1;
	}

	return fread(buffer, 1, count, stdin);
}
