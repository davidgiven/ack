#include "syscall.h"
DEFINE(_geteuid)
SYSNORET(SYS_getuid)
	movl	r1,r0
	ret
