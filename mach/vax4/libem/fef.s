#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.fef

        ! $Header$


.fef:
	movl	(sp)+,r3
	cmpl	r0,$4
	bneq	I1
	movf	(sp)+,r0
	jsb	Ihulp
	movf	r0,-(sp)
	jmp	(r3)
I1:
	cmpl	r0,$8
	bneq	I2
	movd	(sp)+,r0
	jsb	Ihulp
	movd	r0,-(sp)
	jmp	(r3)
I2:
	pushl	$EILLINS
	jmp	.fat
Ihulp:
	extzv	$7,$8,r0,r2
	subl3	$128,r2,-(sp)
	insv	$128,$7,$8,r0
	rsb
