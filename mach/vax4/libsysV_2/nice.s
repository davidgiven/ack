.set	nice,34
.globl	_nice
.globl  cerror

_nice:
	.word	0x0000
	chmk	$nice
	bcc 	ok
	jmp 	cerror
ok:
	ret
