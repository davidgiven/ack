/* $Source$
 * $State$
 * $Revision$
 */

#include <fcntl.h>
#include "libsys.h"

/*
 * Say whether a particular fd is currently open in text mode and has just
 * hit an MS-DOS end-of-file character (^Z).
 */
int _sys_iseof(int fd)
{
	int reqbegfd = fd & ~_FDVECMASK;
	struct _fdmodes *p = &_sys_fdmodes;
	_fdvec_t mask;

	while (p->begfd != reqbegfd)
	{
		p = p->next;
		if (!p)
			return 0;
	}
	mask = (_fdvec_t)1 << (fd & _FDVECMASK);
	return p->eofvec & mask;
}
