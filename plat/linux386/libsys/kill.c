/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

int kill(pid_t pid, int signum)
{
	return _syscall(__NR_kill, pid, signum, 0);
}
