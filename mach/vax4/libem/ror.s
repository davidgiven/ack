#include "em_abs.h"

        # $Header$

.globl .ror

.ror:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    Lerr
	subl3   (sp)+,$32,r0
	rotl    r0,(sp),(sp)
	jmp     (r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
