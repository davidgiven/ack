#include "syscall.h"
DEFINE(_getegid)
SYSNORET(SYS_getgid)
	movl	r1,r0
	ret
