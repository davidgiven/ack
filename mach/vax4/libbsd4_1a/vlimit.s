.globl	_vlimit
.set	vlimit,77

.align	1
_vlimit:
	.word	0x0000
	chmk	$vlimit
	bcc 	1f
	jmp 	errmon
1:
	ret
