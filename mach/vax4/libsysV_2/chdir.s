.set	chdir,12
.globl	_chdir
.globl	cerror

_chdir:
	.word	0x0000
	chmk	$chdir
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
