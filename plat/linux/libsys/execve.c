#include <unistd.h>
#include "libsys.h"

int execve(const char *path, char *const argv[], char *const envp[])
{
	return _syscall(__NR_execve, (quad) path, (quad) argv, (quad) envp);
}
