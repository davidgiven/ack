.globl	_time
.set	time,13

.align	1
_time:
	.word	0x0000
	chmk	$time
	movl	4(ap),r1
	beql	1f
	movl	r0,(r1)
1:
	ret
