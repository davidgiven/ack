        # $Header$
.globl	.cms

	# bytes in r0
.cms:
	movl	(sp)+,r3
	addl3	sp,r0,r1
	addl3	r1,r0,r2
	ashl	$-2,r0,r0
L1:
	cmpl	(sp)+,(r1)+
	bneq	L2
	sobgtr	r0,L1
L2:
	movl	r2,sp
	jmp	(r3)
