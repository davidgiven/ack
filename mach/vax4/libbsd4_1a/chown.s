.globl	_chown
.set	chown,16

.align	1
_chown:
	.word	0x0000
	chmk	$chown
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
