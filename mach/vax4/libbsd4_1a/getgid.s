.globl	_getgid, _getegid
.set	getgid,47

.align	1
_getgid:
	.word	0x0000
	chmk	$getgid
	ret

.align	1
_getegid:
	.word	0x0000
	chmk	$getgid
	movl	r1,r0
	ret
