#include <stdlib.h>
#include <errno.h>
#include <sgtty.h>
#include "libsys.h"

extern struct
{
	int fd;
	char* buffer;
	size_t count;
} _sys_params_in;

extern struct
{
	size_t bytesread;
} _sys_params_out;

#define P _sys_params_in

int _sys_read(void)
{
	char i;
	
	/* We're only allowed to read from fd 0, 1 or 2. */
	
	if ((P.fd < 0) || (P.fd > 2))
		return EBADF;
	
	/* Empty buffer? */
	
	if (P.count == 0)
	{
		_sys_params_out.bytesread = 0;
		return 0;
	}
	
	/* Read one byte. */
	
	i = _sys_rawread();
	if ((i == '\r') && !(_sys_ttyflags & RAW)) 
		i = '\n';
	if (_sys_ttyflags & ECHO)
		_sys_write_tty(i);
	
	*P.buffer = i;
		
	_sys_params_out.bytesread = 1;
	return 0;
}
