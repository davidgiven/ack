.globl	_execle
.globl	_execve

_execle:
	.word	0x0000
	movl	(ap),r0
	pushl	(ap)[r0]
	pushab	8(ap)
	pushl	4(ap)
	calls	$3,_execve
	ret
