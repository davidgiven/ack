.globl	_getuid, _geteuid
.set	getuid,24

.align	1
_getuid:
	.word	0x0000
	chmk	$getuid
	ret

.align	1
_geteuid:
	.word	0x0000
	chmk	$getuid
	movl	r1,r0
	ret
