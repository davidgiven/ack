.globl	_link
.set	link,9

.align	1
_link:
	.word	0x0000
	chmk	$link
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
