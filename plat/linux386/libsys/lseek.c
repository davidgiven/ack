/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

off_t lseek(int fd, off_t offset, int whence)
{
	return _syscall(__NR_lseek, fd, offset, whence);
}
