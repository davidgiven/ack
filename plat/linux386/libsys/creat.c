/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <unistd.h>
#include "libsys.h"

int creat(const char* filename, int mode)
{
	return open(filename, O_CREAT|O_WRONLY|O_TRUNC, mode);
}
