/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

void _exit(int status)
{
	_syscall(__NR_exit, status, 0, 0);
}
