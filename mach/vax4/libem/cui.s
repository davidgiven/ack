#include "em_abs.h"

        # $Header$

.globl .cui

.cui:
	movl    (sp)+,r1
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Lerr
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Lerr
	tstl    (sp)
	bgeq    L1
	jbr     Liovfl
L1:
	jmp     (r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
Liovfl:
	pushl	$EIOVFL
	jmp     .trp
