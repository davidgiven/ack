.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_execle

.align	1
_execle:
	.data2	0x0000
	movl	(ap),r0
	pushl	(ap)[r0]
	pushab	8(ap)
	pushl	4(ap)
	calls	$3,_execve
	ret
