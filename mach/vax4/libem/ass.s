#include "em_abs.h"

        # $Header$

.globl .ass

.ass:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    Lerr
	movl    (sp)+,r0
	addl2   r0,sp
	jmp     (r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
