.set	gtty,32
.globl	_gtty
.globl	cerror

_gtty:
	.word	0x0000
	chmk	$gtty
	bcc	ok
	jmp	cerror
ok:
	ret
