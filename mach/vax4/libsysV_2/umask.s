.set	umask,60
.globl	_umask
.globl	cerror

_umask:
	.word	0x0000
	chmk	$umask
	bcc	ok
	jmp	cerror
ok:
	ret
