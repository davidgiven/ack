#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .rol

        ! $Id$


.rol:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    Ierr
	movl	(sp)+,r0
	rotl    r0,(sp),(sp)
	jmp     (r1)
Ierr:
	pushl	$EILLINS
	jmp     .fat
