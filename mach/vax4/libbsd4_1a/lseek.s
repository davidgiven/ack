.globl	_lseek
.set	lseek,19

.align	1
_lseek:
	.word	0x0000
	chmk	$lseek
	bcc 	1f
	jmp 	errmon
1:
	ret
