#include "em_abs.h"

        # $Header$

.globl	.mlf

.mlf:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	L1
	mulf2	(sp)+,(sp)
	jmp	(r1)
L1:
	cmpl	r0,$8
	bneq	L2
	muld2	(sp)+,(sp)
	jmp	(r1)
L2:
	pushl	$EILLINS
	jmp	.fat
