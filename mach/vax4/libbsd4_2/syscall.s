#include "syscall.h"
DEFINE(_syscall)
	movl	4(ap),r0
	subl3	$1,(ap)+,(ap)
	chmk	r0
	bcs	Ierr
	ret
Ierr:
	jmp	cerror
