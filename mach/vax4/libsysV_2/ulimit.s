.set	ulimit,63
.globl	_ulimit
.globl	cerror

_ulimit:
	.word	0x0000
	chmk	$ulimit
	bcc	ok
	jmp	cerror
ok:
	ret
