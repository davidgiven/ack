.set	chmod,15
.globl	_chmod
.globl	cerror

_chmod:
	.word	0x0000
	chmk	$chmod
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
