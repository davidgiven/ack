.set	times,43
.globl	_times
.globl	cerror

_times:
	.word	0x0000
	chmk	$times
	bcc	ok
	jmp	cerror
ok:
	ret
