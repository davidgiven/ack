#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .sri

        ! $Id$


.sri:
	movl	(sp)+,r2
	cmpl	r0,$4
	bneq	I1
	movl	(sp)+,r0
	mnegl	r0,r0
	movl	(sp)+,r1
	ashl	r0,r1,-(sp)
	jmp	(r2)
I1:
	pushl	$EILLINS
	jmp	.fat
