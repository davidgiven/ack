/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence)
{
	errno = EINVAL;
	return -1;
}
