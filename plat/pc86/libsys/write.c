/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

void _sys_write_tty(char c)
{
	_sys_rawwrite(c);
#if 0
	if ((c == '\n') && !(_sys_ttyflags & RAW))
		_sys_rawwrite('\r');
#endif
	if (c == '\n')
		_sys_rawwrite('\r');
}

ssize_t write(int fd, void* buffer, size_t count)
{
	int i;
	char* p = buffer;
	
	/* We're only allowed to write to fd 0, 1 or 2. */
	
	if ((fd < 0) || (fd > 2))
	{
		errno = EBADF;
		return -1;
	}
	
	/* Write all data. */
	
	i = 0;
	while (i < count)
	{
		_sys_write_tty(*p++);
			
		i++;
	}
	
	/* No failures. */
	
	return count;
}
