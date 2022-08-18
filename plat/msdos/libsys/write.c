/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

ssize_t write(int fd, void* buffer, size_t count)
{
	static const char crlf[2] = "\r\n";
	int i;
	char *p, *q;
	size_t left, n;
	ssize_t r, tot;

	if (!count)
		return 0;

	p = buffer;
	left = count;
	tot = 0;

	if (_sys_getmode(fd) == O_BINARY)
	{
		while (left)
		{
			r = _sys_rawwrite(fd, p, left);
			if (r <= 0)
				return tot ? tot : r;
			tot += r;
			p += r;
			left -= r;
		}
	}
	else
	{
		/* If the file descriptor is an O_TEXT fd, translate LFs to CRLFs. */
		while (left)
		{
			q = memchr(p, '\n', left);
			if (!q)
				return _sys_rawwrite(fd, p, left);

			n = q - p;
			if (n)
			{
				r = _sys_rawwrite(fd, p, n);
				if (r <= 0)
					return tot ? tot : r;
				tot += r;
				p += r;
				left -= r;
				if (r != n)
					break;
			}

			r = _sys_rawwrite(fd, crlf, sizeof crlf);
			if (r != 2)
			{
				if (r > 0)
					r = 0;
				return tot ? tot : r;
			}
			++tot;
			++p;
			--left;
		}
	}
	return tot;
}
