        # $Header$
.globl .cmand

	# bytes in r0

.cmand:
	movl	(sp)+,r2
	addl3	r0,sp,r1
	ashl	$-2,r0,r0
L1:
	bicl2	(sp)+,(r1)+
	sobgtr	r0,L1
	jmp	(r2)
