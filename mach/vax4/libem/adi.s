#include "em_abs.h"

        # $Header$

.globl .adi

.adi:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	L1
	addl2	(sp)+,(sp)
	jmp	(r1)
L1:
	pushl	$EILLINS
	jmp	.fat
