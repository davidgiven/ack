#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .rmi

        ! $Id$


.rmi:
	movl	(sp)+,r3
	cmpl	r0,$4
	bneq	I1
	movl	(sp)+,r2
	movl	(sp)+,r1
	ashq	$-32,r0,r0
	ediv	r2,r0,r1,r0
	pushl	r0
	jmp	(r3)
I1:
	pushl	$EILLINS
	jmp	.fat
