        # $Header$
#include "em_abs.h"

.globl	.dvf

.dvf:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	L1
	divf2	(sp)+,(sp)
	jmp	(r1)
L1:
	cmpl	r0,$8
	bneq	L2
	divd2	(sp)+,(sp)
	jmp	(r1)
L2:
	pushl	$EILLINS
	jmp	.fat
