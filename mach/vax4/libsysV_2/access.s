.set	access,33
.globl	_access
.globl	cerror

_access:
	.word	0x0000
	chmk	$access
	bcc 	ok
	jmp 	cerror
ok:
	ret
