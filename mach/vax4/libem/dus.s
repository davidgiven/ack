#include "em_abs.h"

        # $Header$

.globl .dus

.dus:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    Lerr
	movl    (sp)+,r0
	pushl   r1
	jmp     .dup
Lerr:
	pushl	$EILLINS
	jmp     .fat
