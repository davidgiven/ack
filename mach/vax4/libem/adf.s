#include "em_abs.h"

        # $Header$

.globl	.adf

.adf:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	L1
	addf2	(sp)+,(sp)
	jmp	(r1)
L1:
	cmpl	r0,$8
	bneq	L2
	addd2	(sp)+,(sp)
	jmp	(r1)
L2:
	pushl	$EILLINS
	jmp	.fat
