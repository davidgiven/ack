.set	read,3
.globl	_read
.globl  cerror

_read:
	.word	0x0000
	chmk	$read
	bcc 	ok
	jmp 	cerror
ok:
	ret
