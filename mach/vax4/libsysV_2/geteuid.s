.set	getuid,24
.globl	_geteuid

_geteuid:
	.word	0x0000
	chmk	$getuid
	movl	r1,r0
	ret
