#include "syscall.h"
.extern	_end
DEFINE(_brk)
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

DEFINE(_sbrk)
	addl3	cur,4(ap),-(sp)
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
Imin: .data4	_end
Icur: .data4	_end
