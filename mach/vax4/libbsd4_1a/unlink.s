.globl	_unlink
.set	unlink,10

.align	1
_unlink:
	.word	0x0000
	chmk	$unlink
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
