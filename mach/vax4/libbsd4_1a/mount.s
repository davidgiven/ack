.globl	_mount
.set	mount,21

.align	1
_mount:
	.word	0x0000
	chmk	$mount
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
