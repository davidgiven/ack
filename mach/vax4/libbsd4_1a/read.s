.globl	_read
.set	read,3

.align	1
_read:
	.word	0x0000
	chmk	$read
	bcc 	1f
	jmp 	errmon
1:
	ret
