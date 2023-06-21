/* $Source$
 * $State$
 * $Revision$
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "libsys.h"

sighandler_t signal(int signum, sighandler_t handler)
{
	return SIG_DFL;
}
