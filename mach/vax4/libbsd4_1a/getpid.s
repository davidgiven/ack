.globl	_getpid
.globl	_getppid
.set	getpid,20

.align	1
_getpid:
	.word	0x0000
	chmk	$getpid
	ret

.align	1
_getppid:
	.word	0x0000
	chmk	$getpid
	movl	r1,r0
	ret
