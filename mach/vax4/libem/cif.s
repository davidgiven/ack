#include "em_abs.h"

        # $Header$

.globl  .cif

.cif:
	movl    (sp)+,r1
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Ldbl
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Lerr
	cvtlf   (sp)+,-(sp)
	jmp     (r1)
Ldbl:
	cmpl    r0,$8
	bneq    Lerr
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Lerr
	cvtld   (sp)+,-(sp)
	jmp     (r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
