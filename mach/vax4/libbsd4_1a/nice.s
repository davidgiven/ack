.globl	_nice
.set	nice,34

.align	1
_nice:
	.word	0x0000
	chmk	$nice
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
