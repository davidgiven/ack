.set	kill,37
.globl	_kill
.globl cerror

_kill:
	.word	0x0000
	chmk	$kill
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
