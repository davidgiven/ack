#include "syscall.h"
DEFINE(_getppid)
SYSNORET(SYS_getpid)
	movl	r1,r0
	ret
