        # $Header$
#include "em_abs.h"

.globl .sri

.sri:
	movl	(sp)+,r2
	cmpl	r0,$4
	bneq	L1
	movl	(sp)+,r0
	mnegl	r0,r0
	movl	(sp)+,r1
	ashl	r0,r1,-(sp)
	jmp	(r2)
L1:
	pushl	$EILLINS
	jmp	.fat
