.set	umount,22
.globl	_umount
.globl	cerror

_umount:
	.word	0x0000
	chmk	$umount
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
