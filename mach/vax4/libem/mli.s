        # $Header$
#include "em_abs.h"

.globl .mli

.mli:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	L1
	mull2	(sp)+,(sp)
	jmp	(r1)
L1:
	pushl	$EILLINS
	jmp	.fat
