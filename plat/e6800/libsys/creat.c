/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int creat(const char* path, int mode)
{
	return open(path, O_CREAT|O_WRONLY|O_TRUNC, mode);
}
