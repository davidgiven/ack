.globl	_setgid
.set	setgid,46

.align	1
_setgid:
	.word	0x0000
	chmk	$setgid
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
