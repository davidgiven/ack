.globl	_fstat
.set	fstat,28

.align	1
_fstat:
	.word	0x0000
	chmk	$fstat
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
