#include "syscall.h"
DEFINE(_wait3)
	movl	8(ap),r0
	movl	12(ap),r1
	bispsw	$0xf
	chmk	$SYS_wait
	bcs 	Ierr
	tstl	4(ap)
	beql	Iout
	movl	r1,*4(ap)
Iout:
	ret
Ierr:
	jmp	cerror
