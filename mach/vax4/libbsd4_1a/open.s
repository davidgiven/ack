.globl	_open
.set	open,5

.align	1
_open:
	.word	0x0000
	chmk	$open
	bcc 	1f
	jmp 	errmon
1:
	ret
