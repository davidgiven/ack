.set	pause,29
.globl	_pause
.globl	cerror

_pause:
	.word	0x0000
	chmk	$pause
	bcc	ok
	jmp	cerror
ok:
	clrl	r0
	ret
