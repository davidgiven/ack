.globl	_execl
.globl	_execv

_execl:
	.word	0x0000
	pushab	8(ap)
	pushl	4(ap)
	calls	$2,_execv
	ret
