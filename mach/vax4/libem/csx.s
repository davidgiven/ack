#include "em_abs.h"

        # $Header$

.globl .csa

.csa:
	cmpl    r0,$4
	bneq	Lillins
	jmp	.csa4

.globl .csb

.csb:
	cmpl    r0,$4
	bneq	Lillins
	jmp	.csb4

Lillins:
	pushl	$EILLINS
	jmp     .fat
