#include "syscall.h"
DEFINE(_ptrace)
	clrl	_errno
SYSTEM(SYS_ptrace)
