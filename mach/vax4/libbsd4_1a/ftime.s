.globl	_ftime
.set	ftime,35

.align	1
_ftime:
	.word	0x0000
	chmk	$ftime
	ret
