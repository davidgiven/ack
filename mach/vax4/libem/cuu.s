#include "em_abs.h"

        # $Header$

.globl .cuu

.cuu:
	movl    (sp)+,r1
	cmpl	(sp)+,$4
	bneq	Lerr
	cmpl	(sp)+, $4
	bneq	Lerr
	jmp     (r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
