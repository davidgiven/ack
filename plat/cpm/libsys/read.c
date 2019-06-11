/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <cpm.h>

ssize_t read(int fd, void* buffer, size_t count)
{
	short save;
	unsigned char before_n;

	/* We're only allowed to read from fd 0, 1 or 2. */
	if ((fd < 0) || (fd > 2))
	{
		errno = EBADF;
		return -1;
	}

	/* We need room for at least 1 char plus '\n'. */
	if (count < 2)
	{
		errno = EINVAL;
		return -1;
	}

	/* Make room to append '\n' later. */
	before_n = count > 255 ? 255 : count - 1;

	/* Borrow 2 bytes of RAM before the buffer. */
	/* This might overwrite count!!! */
	save = ((short*)buffer)[-1];

	/* Read one line from the console. */
	((unsigned char*)buffer)[-2] = before_n;
	cpm_readline((uint8_t*)buffer - 2);
	before_n = ((unsigned char*)buffer)[-1];

	((char*)buffer)[before_n] = '\n'; /* Append '\n'. */
	((short*)buffer)[-1] = save; /* Give back borrowed bytes. */

	/* Echo '\n' to console. */
	cpm_printstring("\r\n$");

	return (int)before_n + 1;
}
