/* $Source$
 * $State$
 * $Revision$
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "libsys.h"

off_t lseek(int fd, off_t offset, int whence)
{
	off_t newoff;

	if (whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END)
	{
		errno = EINVAL;
		return -1L;
	}

	newoff = _sys_rawlseek(fd, offset, whence);
	if (newoff != -1L)
		_sys_seteof(fd, 0);

	return newoff;
}
