.set	fcntl,62
.globl	_fcntl
.globl	cerror

_fcntl:
	.word	0x0000
	chmk	$fcntl
	bcc	ok
	jmp	cerror
ok:
	ret
