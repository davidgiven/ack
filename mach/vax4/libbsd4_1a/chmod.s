.globl	_chmod
.set	chmod,15

.align	1
_chmod:
	.word	0x0000
	chmk	$chmod
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
