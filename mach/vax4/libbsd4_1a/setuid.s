.globl	_setuid
.set	setuid,23

.align	1
_setuid:
	.word	0x0000
	chmk	$setuid
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
