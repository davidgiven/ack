.set	close,6
.globl	_close
.globl	cerror

_close:
	.word	0x0000
	chmk	$close
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
