.globl	_pipe
.set	pipe,42

.align	1
_pipe:
	.word	0x0000
	chmk	$pipe
	bcc 	1f
	jmp 	errmon
1:
	movl	4(ap),r2
	movl	r0,(r2)+
	movl	r1,(r2)
	clrl	r0
	ret
