.globl	_sync
.set	sync,36

.align	1	
_sync:
	.word	0x0000
	chmk	$sync
	ret
