#include "syscall.h"
DEFINE(_wait3)
	movl	8(ap),r0
	movl	12(ap),r1
	bispsw	$0xf
	chmk	$SYS_wait
	bcs 	err
	tstl	4(ap)
	beql	out
	movl	r1,*4(ap)
out:
	ret
err:
	jmp	cerror
