.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
pipe = 42
.define	_pipe

_pipe:
	.data2	0x0000
	chmk	$pipe
	bcc 	1f
	jmp 	cerror
1:
	movl	4(ap),r2
	movl	r0,(r2)+
	movl	r1,(r2)
	clrl	r0
	ret
