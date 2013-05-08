/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/write.c,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

int write(int fd, void* buffer, size_t count)
{
	return _syscall(__NR_write, fd, (quad) buffer, count);
}
