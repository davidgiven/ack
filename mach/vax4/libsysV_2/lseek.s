.set	lseek,19
.globl	_lseek
.globl  cerror

_lseek:
	.word	0x0000
	chmk	$lseek
	bcc 	ok
	jmp 	cerror
ok:
	ret
