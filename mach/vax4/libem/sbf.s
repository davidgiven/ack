#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.sbf

        ! $Id$


.sbf:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	I1
	subf2	(sp)+,(sp)
	jmp	(r1)
I1:
	cmpl	r0,$8
	bneq	I2
	subd2	(sp)+,(sp)
	jmp	(r1)
I2:
	pushl	$EILLINS
	jmp	.fat
