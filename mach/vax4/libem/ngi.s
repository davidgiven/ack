#include "em_abs.h"

        # $Header$

.globl .ngi

.ngi:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	L1
	mnegl	(sp),(sp)
	jmp	(r1)
L1:
	pushl	$EILLINS
	jmp	.fat
