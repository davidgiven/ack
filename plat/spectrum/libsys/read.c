/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

ssize_t read(int fd, void* buffer, size_t count)
{
	char i;
	
	/* We're only allowed to read from fd 0, 1 or 2. */
	
	if ((fd < 0) || (fd > 2))
	{
		errno = EBADF;
		return -1;
	}
	
	/* Empty buffer? */
	
	if (count == 0)
		return 0;
	
	/* Read one byte. */
	
	i = _sys_rawread();
#if 0
	if ((i == '\r') && !(_sys_ttyflags & RAW)) 
		i = '\n';
	if (_sys_ttyflags & ECHO)
		_sys_write_tty(i);
#endif
	if (i == '\r') 
		i = '\n';
	_sys_write_tty(i);
	
	*(char*)buffer = i;
	return 1;
}
