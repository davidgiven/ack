.set	wait,7
.globl	_wait

.align	1
_wait:
	.word	0x0000
	chmk	$wait
	bcc 	1f
	jmp	errmon
1:
	tstl	4(ap)
	beql	1f
	movl	r1,*4(ap)
1:
	ret
