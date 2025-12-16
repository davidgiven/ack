/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <emu6800.h>

void _sys_write_tty(unsigned char c)
{
	if (c == '\n')
		emu6800_conout('\r');
	emu6800_conout(c);
}

ssize_t write(int fd, const void* buffer, size_t count)
{
	if (fd == 1)
	{
		/* Write to the console. */

		unsigned char *p = buffer;
		size_t i = count;
		while (i--)
			_sys_write_tty(*p++);
		return count;
	}

eio:
	errno = EIO;
	return -1;
}
