#include "syscall.h"
.globl	_end
DEFINE(_brk)
	cmpl	4(ap),min
	bgeq	ok
	movl	min,4(ap)
ok:
	chmk	$17
	bcs	err
	movl	4(ap),cur
	clrl	r0
	ret
err:
	jmp	cerror

DEFINE(_sbrk)
	addl3	cur,4(ap),-(sp)
	bcs	err
	pushl	$1
	movl	ap,r3
	movl	sp,ap
	chmk	$17
	bcs	err
	movl	cur,r0
	addl2	4(r3),cur
	ret
.data
min: .long	_end
cur: .long	_end
