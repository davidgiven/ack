.globl	_execv
.globl	_environ

.align	1
_execv:
	.word	0x0000
	pushl	_environ
	pushl	8(ap)
	pushl	4(ap)
	calls	$3,_execve
	ret
