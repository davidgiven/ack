        # $Header$
.globl	.and

	# bytes in r0
.and:
	movl	(sp)+,r3
	addl3	r0,sp,r1
	ashl	$-2,r0,r0
L1:
	mcoml	(sp)+,r2
	bicl2	r2,(r1)+
	sobgtr	r0,L1
	jmp	(r3)
