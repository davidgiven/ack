#include "em_abs.h"

        # $Header$

.globl .rmi

.rmi:
	movl	(sp)+,r3
	cmpl	r0,$4
	bneq	L1
	movl	(sp)+,r2
	movl	(sp)+,r1
	ashq	$-32,r0,r0
	ediv	r2,r0,r1,r0
	pushl	r0
	jmp	(r3)
L1:
	pushl	$EILLINS
	jmp	.fat
