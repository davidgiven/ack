.globl	_creat
.set	creat,8

.align	1
_creat:
	.word	0x0000
	chmk	$creat
	bcc 	1f
	jmp 	errmon
1:
	ret
