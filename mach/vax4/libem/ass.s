#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .ass

        ! $Id$


.ass:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    1f
	movl    (sp)+,r0
	addl2   r0,sp
	jmp     (r1)
1:
	pushl	$EILLINS
	jmp     .fat
