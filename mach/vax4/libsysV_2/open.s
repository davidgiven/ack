.set	open,5
.globl	_open
.globl  cerror

_open:
	.word	0x0000
	chmk	$open
	bcc 	ok
	jmp 	cerror
ok:
	ret
