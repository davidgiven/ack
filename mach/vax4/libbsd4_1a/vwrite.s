.globl	_vwrite
.set	vwrite,68

.align	1
_vwrite:
	.word	0x0000
	chmk	$vwrite
	bcc 	1f
	jmp 	errmon
1:
	ret
