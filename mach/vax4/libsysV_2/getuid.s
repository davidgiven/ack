.set	getuid,24
.globl	_getuid

_getuid:
	.word	0x0000
	chmk	$getuid
	ret
