.set	creat,8
.globl	_creat
.globl	cerror

_creat:
	.word	0x0000
	chmk	$creat
	bcc 	ok
	jmp 	cerror
ok:
	ret
