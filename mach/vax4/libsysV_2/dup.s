.set	dup,41
.globl	_dup
.globl	cerror

_dup:
	.word	0x0000
	chmk	$dup
	bcc 	ok
	jmp 	cerror
ok:
	ret
