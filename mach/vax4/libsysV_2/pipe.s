.set	pipe,42
.globl	_pipe
.globl  cerror

_pipe:
	.word	0x0000
	chmk	$pipe
	bcc 	ok
	jmp 	cerror
ok:
	movl	4(ap),r2
	movl	r0,(r2)+
	movl	r1,(r2)
	clrl	r0
	ret
