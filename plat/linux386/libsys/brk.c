/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <unistd.h>
#include "libsys.h"

int brk(void* end)
{
	return _syscall(__NR_brk, (quad) end, 0, 0);
}
