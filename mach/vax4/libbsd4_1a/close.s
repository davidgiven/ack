.globl	_close
.set	close,6

.align	1
_close:
	.word	0x0000
	chmk	$close
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
