.set	setgid,46
.globl	_setgid
.globl  cerror

_setgid:
	.word	0x0000
	chmk	$setgid
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
