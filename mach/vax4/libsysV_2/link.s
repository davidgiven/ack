.set	link,9
.globl	_link
.globl	cerror

_link:
	.word	0x0000
	chmk	$link
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
