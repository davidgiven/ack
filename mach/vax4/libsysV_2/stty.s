.set	stty,31
.globl	_stty
.globl	cerror

_stty:
	.word	0x0000
	chmk	$stty
	bcc	ok
	jmp	cerror
ok:
	ret
