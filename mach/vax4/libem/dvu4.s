#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .dvu4

        ! $Id$


.dvu4:
	movl    (sp)+,r3
	movl    (sp)+,r2
	blss    I1
	movl    (sp)+,r0
	clrl    r1
	ediv    r2,r0,r0,r1
	jmp     (r3)
I1:
	cmpl    (sp)+,r2
	bgequ   I2
	clrl	r0
	jmp     (r3)
I2:
	movl	$1,r0
	jmp     (r3)
