#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.fif

        ! $Id$

.sect .rom
Ione: .data1 0200, 077, 00, 00, 00, 00, 00, 00

.fif:
	movl	(sp)+,r2
	cmpl	r0,$4
	bneq	I1
	mulf3	(sp)+,(sp)+,r0
	emodf	r0,$0,Ione,r1,-(sp)
	subf3	(sp),r0,-(sp)
	jmp	(r2)
I1:
	cmpl	r0,$8
	bneq	I2
	muld3	(sp)+,(sp)+,r0
	emodd	r0,$0,Ione,r0,-(sp)
	subd3	(sp),r0,-(sp)
	jmp	(r2)
I2:
	pushl	$EILLINS
	jmp	.fat
