.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_syscall

_syscall:
	.data2	0x0000
	movl	4(ap),r0
	subl3	$1,(ap)+,(ap)
	chmk	r0
	bcc	1f
	jmp	cerror
1:
	ret
