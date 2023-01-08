/* $Source$
 * $State$
 * $Revision$
 */

#include <fcntl.h>
#include "libsys.h"

/*
 * Set text/binary mode for a particular fd.  Assume that the fd is valid,
 * and that the mode is either O_TEXT or O_BINARY.
 *
 * Return the previous mode (either O_TEXT or O_BINARY) on success, -1 (with
 * errno set) on error.
 */
int _sys_setmode(int fd, int mode)
{
	int reqbegfd = fd & ~_FDVECMASK;
	struct _fdmodes *p = &_sys_fdmodes;
	_fdvec_t mask;

	while (p->begfd != reqbegfd)
	{
		p = p->next;
		if (!p)
		{
			if (mode == O_TEXT)
				return O_TEXT;
			if ((p = sbrk(sizeof(struct _fdmodes)))
			    == OUT_OF_MEMORY)
				return -1;
			p->next = _sys_fdmodes.next;
			p->begfd = reqbegfd;
			p->modevec = p->eofvec = 0;
			_sys_fdmodes.next = p;
			break;
		}
	}
	mask = (_fdvec_t)1 << (fd & _FDVECMASK);
	if (mode == O_BINARY)
	{
		if (p->modevec & mask)
			return O_BINARY;
		p->modevec |= mask;
		p->eofvec &= ~mask;
		return O_TEXT;
	}
	else
	{
		if (!(p->modevec & mask))
			return O_TEXT;
		p->modevec &= ~mask;
		return O_BINARY;
	}
}
