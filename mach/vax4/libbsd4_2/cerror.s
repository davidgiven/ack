	.globl	_errno
cerror:
	movl	r0,_errno
	mnegl	$1,r0
	ret
