#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .sli

        ! $Id$


.sli:
	movl	(sp)+,r2
	cmpl	r0,$4
	bneq	I1
	movl	(sp)+,r0
	movl	(sp)+,r1
	ashl	r0,r1,-(sp)
	jmp	(r2)
I1:
	pushl	$EILLINS
	jmp	.fat
