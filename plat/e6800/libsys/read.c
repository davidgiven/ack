/* $Source$
 * $State$
 * $Revision$
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <emu6800.h>

ssize_t read(int fd, void* buffer, size_t count)
{
	if (fd == 0)
	{
		/* Read from the console. */

		if (count == 0)
			return 0;
		*(uint8_t*)buffer = emu6800_conin();
		return 1;
	}

eio:
	errno = EIO;
	return -1;
}
