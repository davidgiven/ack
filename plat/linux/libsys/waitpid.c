#include <sys/wait.h>
#include "libsys.h"

pid_t waitpid(pid_t pid, int *wstatus, int options)
{
	return (pid_t) _syscall(__NR_waitpid, pid, (quad) wstatus, options);
}
