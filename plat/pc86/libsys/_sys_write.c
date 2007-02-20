#include <stdlib.h>
#include <errno.h>
#include <sgtty.h>
#include "libsys.h"

extern struct
{
	int fd;
	const char* buffer;
	size_t count;
} _sys_params_in;

extern struct
{
	size_t byteswritten;
} _sys_params_out;

#define P _sys_params_in

void _sys_write_tty(char c)
{
	_sys_rawwrite(c);
	if ((c == '\n') && !(_sys_ttyflags & RAW))
		_sys_rawwrite('\r');
}

int _sys_write(void)
{
	int i;
	
	/* We're only allowed to write to fd 0, 1 or 2. */
	
	if ((P.fd < 0) || (P.fd > 2))
		return EBADF;
	
	/* Write all data. */
	
	i = 0;
	while (i < P.count)
	{
		_sys_write_tty(*P.buffer++);
			
		i++;
	}
	
	/* No failures. */
	
	_sys_params_out.byteswritten = P.count;
	return 0;
}
