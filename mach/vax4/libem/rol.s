        # $Header$
#include "em_abs.h"

.globl .rol

.rol:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    Lerr
	movl	(sp)+,r0
	rotl    r0,(sp),(sp)
	jmp     (r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
