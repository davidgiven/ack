#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .ngi

        ! $Id$


.ngi:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	I1
	mnegl	(sp),(sp)
	jmp	(r1)
I1:
	pushl	$EILLINS
	jmp	.fat
