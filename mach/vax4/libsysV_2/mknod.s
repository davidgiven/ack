.set	mknod,14
.globl	_mknod
.globl  cerror

_mknod:
	.word	0x0000
	chmk	$mknod
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
