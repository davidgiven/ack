#include "em_abs.h"

        # $Header$

.globl .dvu

.dvu:
	cmpl    r0,$4
	bneq	Lerr
	jmp	.dvu4
Lerr:
	pushl	$EILLINS
	jmp     .fat
