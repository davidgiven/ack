.globl	_access
.set	access,33

.align	1
_access:
	.word	0x0000
	chmk	$access
	bcc 	1f
	jmp 	errmon
1:
	ret
