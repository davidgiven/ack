.set	fstat,28
.globl	_fstat
.globl	cerror

_fstat:
	.word	0x0000
	chmk	$fstat
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
