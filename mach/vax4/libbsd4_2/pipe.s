#include "syscall.h"
DEFINE(_pipe)
SYSNORET(SYS_pipe)
	movl	4(ap),r2
	movl	r0,(r2)+
	movl	r1,(r2)
	clrl	r0
	ret
