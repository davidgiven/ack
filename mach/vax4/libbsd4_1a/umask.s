.globl	_umask
.set	umask,60

.align	1
_umask:
	.word	0x0000
	chmk	$umask
	bcc	1f
	jmp	errmon
1:
	ret
