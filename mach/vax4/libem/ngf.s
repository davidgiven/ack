        # $Header$
#include "em_abs.h"

.globl	.ngf

.ngf:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	L1
	mnegf	(sp),(sp)
	jmp	(r1)
L1:
	cmpl	r0,$8
	bneq	L2
	mnegd	(sp),(sp)
	jmp	(r1)
L2:
	pushl	$EILLINS
	jmp	.fat
