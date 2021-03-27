/* $Source$
 * $State$
 * $Revision$
 */

#include <fcntl.h>
#include "libsys.h"

/*
 * Say whether a particular fd is currently open in text or binary mode.
 * Assume that the fd is valid.  Return O_TEXT or O_BINARY.
 */
int _sys_getmode(int fd)
{
	int reqbegfd = fd & ~_FDVECMASK;
	struct _fdmodes *p = &_sys_fdmodes;
	_fdvec_t mask;

	while (p->begfd != reqbegfd)
	{
		p = p->next;
		if (!p)
			return O_TEXT;
	}
	mask = (_fdvec_t)1 << (fd & _FDVECMASK);
	return (p->modevec & mask) ? O_BINARY : O_TEXT;
}
