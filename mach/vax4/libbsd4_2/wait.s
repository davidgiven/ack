#include "syscall.h"
DEFINE(_wait)
SYSNORET(SYS_wait)
	tstl	4(ap)
	beql	out
	movl	r1,*4(ap)
out:
	ret
