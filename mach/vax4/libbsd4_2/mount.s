#include "syscall.h"
DEFINE(_mount)
SYSNORET(SYS_mount)
	clrl	r0
	ret
