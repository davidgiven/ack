/* $Source$
 * $State$
 * $Revision$
 */

#include <fcntl.h>
#include "libsys.h"

/*
 * Set or reset the "end-of-file encountered" indicator for a particular fd.
 * Assume that the fd is valid, and is being read from in text mode.
 */
int _sys_seteof(int fd, int eof)
{
	int reqbegfd = fd & ~_FDVECMASK;
	struct _fdmodes *p = &_sys_fdmodes;
	_fdvec_t mask;

	while (p->begfd != reqbegfd)
	{
		p = p->next;
		if (!p)
		{
			if (!eof)
				return 0;
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
	if (eof)
		p->eofvec |= mask;
	else
		p->eofvec &= ~mask;
	return 0;
}
