#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .cuu

        ! $Id$


.cuu:
	movl    (sp)+,r1
	cmpl	(sp)+,$4
	bneq	Ierr
	cmpl	(sp)+, $4
	bneq	Ierr
	jmp     (r1)
Ierr:
	pushl	$EILLINS
	jmp     .fat
