#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .rck

        ! $Id$


.rck:
	cmpl    r0,$4
	bneq	Ierr
	jmp	.rck4
Ierr:
	pushl	$EILLINS
	jmp     .fat
