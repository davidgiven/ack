#include "syscall.h"
DEFINE(__brk)
	cmpl	4(ap),$endbss
	bgeq	Iok
	movl	$endbss,4(ap)
Iok:
	chmk	$17
	bcs	Ierr
	movl	4(ap),.limhp
	clrl	r0
	ret
Ierr:
	jmp	cerror

DEFINE(__sbrk)
	addl3	.limhp,4(ap),-(sp)
	bcs	Ierr
	pushl	$1
	movl	ap,r3
	movl	sp,ap
	chmk	$17
	bcs	Ierr
	movl	.limhp,r0
	addl2	4(r3),.limhp
	ret
