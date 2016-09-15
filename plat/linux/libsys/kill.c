/* $Source: /cvsroot/tack/Ack/plat/linux386/libsys/kill.c,v $
 * $State: Exp $
 * $Revision: 1.1 $
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

int kill(pid_t pid, int signum)
{
	return _syscall(__NR_kill, pid, signum, 0);
}
