#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .cmu

        ! $Id$


.cmu:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    Ierr
	clrl    r0
	cmpl    (sp)+,(sp)+
	beqlu   I1
	bgtru   I2
	incl    r0
	br     I1
I2:
	decl    r0
I1:
	jmp     (r1)
Ierr:
	pushl	$EILLINS
	jmp     .fat
