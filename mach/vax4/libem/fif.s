        # $Header$
#include "em_abs.h"

.globl	.fif

.fif:
	movl	(sp)+,r2
	cmpl	r0,$4
	bneq	L1
	mulf3	(sp)+,(sp)+,r0
	emodf	r0,$0,$0f1.0,r1,-(sp)
	subf3	(sp),r0,-(sp)
	jmp	(r2)
L1:
	cmpl	r0,$8
	bneq	L2
	muld3	(sp)+,(sp)+,r0
	emodd	r0,$0,$0f1.0,r0,-(sp)
	subd3	(sp),r0,-(sp)
	jmp	(r2)
L2:
	pushl	$EILLINS
	jmp	.fat
