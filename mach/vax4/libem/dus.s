#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .dus

        ! $Id$


.dus:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    Ierr
	movl    (sp)+,r0
	pushl   r1
	jmp     .dup
Ierr:
	pushl	$EILLINS
	jmp     .fat
