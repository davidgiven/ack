.globl	_fork
.set	fork,2

.align	1
_fork:
	.word	0x0000
	chmk	$fork
	bcc	1f
	jmp	errmon
1:
	jlbc	r1,1f
	clrl	r0
1:
	ret
