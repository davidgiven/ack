        # $Header$
.globl	.inn

	# bytes in r0
.inn:
	movl	(sp)+,r3
	movl	(sp)+,r1
	ashl	$3,r0,r2	# nr of bits in r2
	cmpl	r1,r2
	bgequ	L2
	bbc	r1,(sp),L2
	movl	$1,r1
L1:
	addl2	r0,sp
	jmp	(r3)
L2:
	clrl	r1
	brb	L1
