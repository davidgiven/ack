#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .cui

        ! $Id$


.cui:
	movl    (sp)+,r1
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Ierr
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Ierr
	jmp     (r1)
Ierr:
	pushl	$EILLINS
	jmp     .fat
