.globl	_vread
.set	vread,67

.align	1
_vread:
	.word	0x0000
	chmk	$vread
	bcc 	1f
	jmp 	errmon
1:
	ret
