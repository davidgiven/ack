        # $Header$
#include "em_abs.h"

.globl .dvi

.dvi:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	L1
	divl2	(sp)+,(sp)
	jmp	(r1)
L1:
	pushl	$EILLINS
	jmp	.fat
