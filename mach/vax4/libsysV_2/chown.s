.set	chown,16
.globl	_chown
.globl	cerror

_chown:
	.word	0x0000
	chmk	$chown
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
