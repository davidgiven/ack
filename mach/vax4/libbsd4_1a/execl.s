.globl	_execl
.globl	_environ

.align	1
_execl:
	.word	0x0000
	pushl	_environ
	pushab	8(ap)
	pushl	4(ap)
	calls	$3,_execve
	ret
