#include "em_abs.h"

        # $Header$

.globl .sli

.sli:
	movl	(sp)+,r2
	cmpl	r0,$4
	bneq	L1
	movl	(sp)+,r0
	movl	(sp)+,r1
	ashl	r0,r1,-(sp)
	jmp	(r2)
L1:
	pushl	$EILLINS
	jmp	.fat
