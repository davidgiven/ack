#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .lar
.define  .sar
.define  .aar

        ! $Id$


.lar:
	cmpl    r0,$4
	bneq    Iillins
	jmp     .lar4

.sar:
	cmpl    r0,$4
	bneq    Iillins
	jmp     .sar4

.aar:
	cmpl    r0,$4
	bneq    Iillins
	jmp     .aar4

Iillins:
	pushl	$EILLINS
	jmp     .fat
