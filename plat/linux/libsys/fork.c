#include <sys/types.h>
#include <unistd.h>
#include "libsys.h"

pid_t fork(void)
{
	return (pid_t) _syscall(__NR_fork, 0, 0, 0);
}
