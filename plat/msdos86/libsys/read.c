/* $Source$
 * $State$
 * $Revision$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

ssize_t read(int fd, void* buffer, size_t count)
{
	char *p, *q;
	ssize_t r, tot;
	size_t left;
	int eof = 0;

	if (!count || _sys_getmode(fd) == O_BINARY)
		return _sys_rawread(fd, buffer, count);

	if (_sys_iseof(fd))
		return 0;

	/*
	 * If the file descriptor is an O_TEXT fd, read from it, and then
	 * remove CRs from the input.  After removing CRs, if the buffer is
	 * not filled and the fd still has bytes left to read, then keep
	 * reading, and keep removing CRs.
	 *
	 * Also handle end-of-file indicators (^Z).  If we see one, move the
	 * file pointer to just before the ^Z.  Also set an internal flag
	 * for the fd so that we do not try to read any further (until e.g. 
	 * a seek happens).
	 */
	p = buffer;
	tot = 0;

	do
	{
		r = _sys_rawread(fd, p, count - (p - (char *)buffer));
		if (r <= 0)
			return tot;

		q = memchr(p, 0x1a, (size_t)r);
		if (q)
		{
			_sys_rawlseek(fd, (off_t)(q - p) - r, SEEK_CUR);
			r = q - p;
			eof = 1;
			_sys_seteof(fd, 1);
		}

		q = memchr(p, '\r', (size_t)r);
		if (!q)
			return tot + r;

		tot += q - p;
		left = r - (q + 1 - p);
		p = q;
		++q;

		while (left)
		{
			char c = *q++;
			if (c != '\r')
			{
				*p++ = c;
				++tot;
			}
			--left;
		}
	} while (tot < count && !eof && _sys_isreadyr(fd));

	return tot;
}
