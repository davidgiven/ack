        # $Header$
#include "em_abs.h"

.globl	.fef

.fef:
	movl	(sp)+,r3
	cmpl	r0,$4
	bneq	L1
	movf	(sp)+,r0
	jsb	Lhulp
	movf	r0,-(sp)
	jmp	(r3)
L1:
	cmpl	r0,$8
	bneq	L2
	movd	(sp)+,r0
	jsb	Lhulp
	movd	r0,-(sp)
	jmp	(r3)
L2:
	pushl	$EILLINS
	jmp	.fat
Lhulp:
	extzv	$7,$8,r0,r2
	subl3	$128,r2,-(sp)
	insv	$128,$7,$8,r0
	rsb
