/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/lseek.c,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

off_t lseek(int fd, off_t offset, int whence)
{
	return _syscall(__NR_lseek, fd, offset, whence);
}
