.globl	_kill
.set	kill,37

.align	1
_kill:
	.word	0x0000
	chmk	$kill
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
