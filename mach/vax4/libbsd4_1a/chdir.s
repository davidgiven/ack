.globl	_chdir
.set	chdir,12

.align	1
_chdir:
	.word	0x0000
	chmk	$chdir
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
