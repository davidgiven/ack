#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .csa
.define .csb

        ! $Id$


.csa:
	cmpl    r0,$4
	bneq	Iillins
	jmp	.csa4

.csb:
	cmpl    r0,$4
	bneq	Iillins
	jmp	.csb4

Iillins:
	pushl	$EILLINS
	jmp     .fat
