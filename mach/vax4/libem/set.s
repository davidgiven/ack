#include "em_abs.h"

        # $Header$

.globl	.setx

	# bytes in r0
.setx:
	movl	(sp)+,r3
	movl	(sp)+,r1	# bitnumber in r1
	subl3	r0,sp,r2
L1:
	clrl	-(sp)
	cmpl	sp,r2
	bgtru	L1
	ashl	$3,r0,r2	# number of bits in r2
	cmpl	r1,r2
	bgequ	L2
	bbcs	r1,(sp),L3
L3:
	jmp	(r3)
L2:
	pushl	$ESET
	jsb	.trp
	jmp	(r3)
