#include <sys/wait.h>
#include "libsys.h"

pid_t wait(int *wstatus)
{
	return (pid_t) _syscall(__NR_waitpid, -1, (quad) wstatus, 0);
}
