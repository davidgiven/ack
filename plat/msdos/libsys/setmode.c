/* $Source$
 * $State$
 * $Revision$
 */

#include <errno.h>
#include <fcntl.h>
#include "libsys.h"

int _setmode(int fd, int mode)
{
	if (mode != O_TEXT && mode != O_BINARY)
	{
		errno = EINVAL;
		return -1;
	}
	if (!_sys_isopen(fd))
	{
		errno = EBADF;
		return -1;
	}
	return _sys_setmode(fd, mode);
}
