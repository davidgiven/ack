#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .dvi

        ! $Id$


.dvi:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	I1
	divl2	(sp)+,(sp)
	jmp	(r1)
I1:
	pushl	$EILLINS
	jmp	.fat
