.set	plock,45
.globl	_plock
.globl	cerror

_plock:
	.word	0x0000
	chmk	$plock
	bcc	ok
	jmp	cerror
ok:
	ret
