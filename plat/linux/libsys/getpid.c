/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/getpid.c,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

pid_t getpid(void)
{
	return _syscall(__NR_getpid, 0, 0, 0);
}
