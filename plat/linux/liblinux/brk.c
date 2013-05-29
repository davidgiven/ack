/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/brk.c,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "libsys.h"

int brk(void* end)
{
	int e = _syscall(__NR_brk, (quad) end, 0, 0);
	if (e == -1)
		errno = ENOMEM;
	return e;
}
