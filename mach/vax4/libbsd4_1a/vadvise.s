.set	vadvise,72
.globl	_vadvise

.align	1
_vadvise:
	.word	0x0000
	chmk	$vadvise
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
