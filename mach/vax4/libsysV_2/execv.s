.globl	_execv
.globl	_execve
.globl	_environ

_execv:
	.word	0x0000
	pushl	_environ
	pushl	8(ap)
	pushl	4(ap)
	calls	$3,_execve
	ret
