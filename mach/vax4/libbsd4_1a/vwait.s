.globl	_vwait
.set	vwait,71

.align	1
_vwait:
	.word	0x0000
	chmk	$vwait
	bcc 	1f
	jmp 	errmon
1:
	tstl	4(ap)
	beql	1f
	movl	r1,*4(ap)
1:
	ret
