.set	unlink,10
.globl	_unlink
.globl  cerror

_unlink:
	.word	0x0000
	chmk	$unlink
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
