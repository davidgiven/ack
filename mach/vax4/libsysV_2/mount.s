.set	mount,21
.globl	_mount
.globl  cerror

_mount:
	.word	0x0000
	chmk	$mount
	bcc	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
