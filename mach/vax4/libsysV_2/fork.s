.set	fork,2
.globl	_fork
.globl	cerror

_fork:
	.word	0x0000
	chmk	$fork
	bcc	ok
	jmp	cerror
ok:
	blbc	r1,out
	clrl	r0
out:
	ret
