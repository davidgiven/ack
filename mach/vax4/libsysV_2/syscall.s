.globl	_syscall
.globl	cerror

_syscall:
	.word	0x0000
	movl	4(ap),r0
	subl3	$1,(ap)+,(ap)
	chmk	r0
	bcc	ok
	jmp	cerror
ok:
	ret
