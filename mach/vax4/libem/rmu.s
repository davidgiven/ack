#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .rmu

        ! $Id$


.rmu:
	cmpl    r0,$4
	bneq	Ierr
	jsb	.rmu4
	pushl	r0
	rsb
Ierr:
	pushl	$EILLINS
	jmp     .fat
