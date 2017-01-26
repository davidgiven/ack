#include <signal.h>
#include "libsys.h"

int sigprocmask(int flags, const sigset_t *new, sigset_t *old)
{
	return _syscall(__NR_sigprocmask, flags, (quad) new, (quad) old);
}
