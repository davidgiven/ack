#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.fef

        ! $Id$


.fef:
	movl	(sp)+,r3
	cmpl	r0,$4
	bneq	I1
	cvtfd	(sp),r0
	jsb	Ihulp
	cvtdf	r0,(sp)
	movl	r2,-(sp)
	jmp	(r3)
I1:
	cmpl	r0,$8
	bneq	I2
	movd	(sp),r0
	jsb	Ihulp
	movd	r0,(sp)
	movl	r2,-(sp)
	jmp	(r3)
I2:
	pushl	$EILLINS
	jmp	.fat
Ihulp:
	tstd	r0
	bneq	1f
	clrl	r2
	rsb
1:
	extzv	$7,$8,r0,r2
	subl2	$128,r2
	insv	$128,$7,$8,r0
	rsb
