.set	setuid,23
.globl	_setuid
.globl  cerror

_setuid:
	.word	0x0000
	chmk	$setuid
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
