#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.adf

        ! $Id$


.adf:
	movl	(sp)+,r1
	cmpl	r0,$4
	bneq	1f
	addf2	(sp)+,(sp)
	jmp	(r1)
1:
	cmpl	r0,$8
	bneq	2f
	addd2	(sp)+,(sp)
	jmp	(r1)
2:
	pushl	$EILLINS
	jmp	.fat
