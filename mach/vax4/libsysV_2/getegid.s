.set	getgid,47
.globl	_getegid

_getegid:
	.word	0x0000
	chmk	$getgid
	movl	r1,r0
	ret
