.globl	_mknod
.set	mknod,14

.align	1
_mknod:
	.word	0x0000
	chmk	$mknod
	bcc 	1f
	jmp 	errmod
1:
	clrl	r0
	ret
