	.globl	_errno, cerror
.comm _errno, 4
cerror:
	movl	r0,_errno
	mnegl	$1,r0
	ret

