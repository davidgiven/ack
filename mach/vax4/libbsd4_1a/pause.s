.globl	_pause
.set	pause,29

.align	1
_pause:
	.word	0x0000
	chmk	$pause
	ret
