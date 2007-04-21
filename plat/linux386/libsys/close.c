/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

int close(int fd)
{
	return _syscall(__NR_close, fd, 0, 0);
}
