#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .adi

        ! $Id$


.adi:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	1f
	addl2	(sp)+,(sp)
	jmp	(r1)
1:
	pushl	$EILLINS
	jmp	.fat
