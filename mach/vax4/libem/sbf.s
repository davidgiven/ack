#include "em_abs.h"

        # $Header$

.globl	.sbf

.sbf:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	L1
	subf2	(sp)+,(sp)
	jmp	(r1)
L1:
	cmpl	r0,$8
	bneq	L2
	subd2	(sp)+,(sp)
	jmp	(r1)
L2:
	pushl	$EILLINS
	jmp	.fat
