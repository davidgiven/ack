.globl	_times
.set	times,43

.align	1
_times:
	.word	0x0000
	chmk	$times
	ret
