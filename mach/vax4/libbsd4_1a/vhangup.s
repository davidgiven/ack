.globl	_vhangup
.set	vhangup,76

.align	1
_vhangup:
	.word	0x0000
	chmk	$vhangup
	ret
