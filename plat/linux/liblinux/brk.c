/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/brk.c,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#include <stdlib.h>
#include <unistd.h>
#include "libsys.h"

int brk(void* end)
{
	return _syscall(__NR_brk, (quad) end, 0, 0);
}
