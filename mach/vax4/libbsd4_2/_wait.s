#include "syscall.h"
DEFINE(__wait)
SYSNORET(SYS_wait)
	tstl	4(ap)
	beql	Iout
	movl	r1,*4(ap)
Iout:
	ret
