.globl	_write
.set	write,4

.align	1
_write:
	.word	0x0000
	chmk	$write
	bcc	1f
	jmp	errmon
1:
	ret
