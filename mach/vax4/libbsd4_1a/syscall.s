.globl	_syscall

.align	1
_syscall:
	.word	0x0000
	movl	4(ap),r0
	subl3	$1,(ap)+,(ap)
	chmk	r0
	bcc	1f
	jmp	errmon
1:
	ret
