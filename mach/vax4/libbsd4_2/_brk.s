#include "syscall.h"
DEFINE(__brk)
	cmpl	4(ap),Imin
	bgeq	Iok
	movl	Imin,4(ap)
Iok:
	chmk	$17
	bcs	Ierr
	movl	4(ap),Icur
	clrl	r0
	ret
Ierr:
	jmp	cerror

DEFINE(__sbrk)
	addl3	Icur,4(ap),-(sp)
	bcs	Ierr
	pushl	$1
	movl	ap,r3
	movl	sp,ap
	chmk	$17
	bcs	Ierr
	movl	Icur,r0
	addl2	4(r3),Icur
	ret
.sect .data
Imin: .data4	endbss
Icur: .data4	endbss
