/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

pid_t getpid(void)
{
	return _syscall(__NR_getpid, 0, 0, 0);
}
