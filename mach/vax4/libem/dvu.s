#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .dvu

        ! $Id$


.dvu:
	cmpl    r0,$4
	bneq	Ierr
	jmp	.dvu4
Ierr:
	pushl	$EILLINS
	jmp     .fat
